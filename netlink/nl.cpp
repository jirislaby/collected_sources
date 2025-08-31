#include <iomanip>
#include <iostream>
#include <netlink/netlink.h>
#include <netlink/socket.h>
#include <netlink/msg.h>
#include <netlink/handlers.h>
#include <netlink/attr.h>
#include <netlink/route/link.h>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/genl.h>

#include <sl/helpers/Color.h>
#include <sl/helpers/Unique.h>
#include <linux/ovpn.h>

using Clr = SlHelpers::Color;

using SockHolder = SlHelpers::UniqueHolder<struct nl_sock>;
using CacheHolder = SlHelpers::UniqueHolder<struct nl_cache>;
using MsgHolder = SlHelpers::UniqueHolder<struct nl_msg>;
using CBHolder = SlHelpers::UniqueHolder<struct nl_cb>;
using FamilyHolder = SlHelpers::UniqueHolder<struct genl_family>;

template<>
void SlHelpers::Deleter<struct nl_sock>::operator()(struct nl_sock *sock) const
{
	::nl_close(sock);
	::nl_socket_free(sock);
}

template<>
void SlHelpers::Deleter<struct nl_cache>::operator()(struct nl_cache *cache) const
{
	::nl_cache_free(cache);
}

template<>
void SlHelpers::Deleter<struct nl_msg>::operator()(struct nl_msg *msg) const
{
	::nlmsg_free(msg);
}

template<>
void SlHelpers::Deleter<struct nl_cb>::operator()(struct nl_cb *cb) const
{
	::nl_cb_put(cb);
}

template<>
void SlHelpers::Deleter<struct genl_family>::operator()(struct genl_family *family) const
{
	::genl_family_put(family);
}

static void __attribute_maybe_unused__
dump(const std::string &prefix, const void *mem, size_t len, bool hex = false)
{
	auto past = reinterpret_cast<const char *>(mem);
	std::cout << prefix << ':';
	if (!hex)
		std::cout << ' ';
	for (auto i = 0U; i < len; ++i) {
		if (hex)
			std::cout << " 0x" << std::hex << std::setfill('0') << std::setw(2) << (unsigned)(unsigned char)past[i];
		else
			std::cout << (std::isprint(past[i]) ? past[i] : '.');
	}
	std::cout << std::resetiosflags(std::ios_base::basefield) << '\n';
}

class Route {
public:
	static int validCB(struct nl_msg *msg, void *);
	static int inCB(struct nl_msg *msg, void *);
	static int errorCB(struct sockaddr_nl *, struct nlmsgerr *e, void *);

	static int run(bool fail);
};

int Route::validCB(struct nl_msg *msg, void *)
{
	struct nlmsghdr *hdr = nlmsg_hdr(msg);
	struct ifinfomsg *ifi;

	std::cout << __func__ << '\n';

	if (!nlmsg_valid_hdr(hdr, sizeof(*ifi)))
		return -NLE_MSG_TOOSHORT;

	ifi = reinterpret_cast<struct ifinfomsg *>(nlmsg_data(hdr));

	auto nameattr = nla_find(nlmsg_attrdata(hdr, sizeof(*ifi)),
				 nlmsg_attrlen(hdr, sizeof(*ifi)), IFLA_IFNAME);
	auto name = nameattr ? reinterpret_cast<const char *>(nla_data(nameattr)) : "(null)";

	std::cout << "ifindex=" << ifi->ifi_index << " name=" << name << '\n';

	return NL_OK;
}

int Route::inCB(struct nl_msg *msg, void *)
{
	auto hdr = nlmsg_hdr(msg);
	if (hdr->nlmsg_type != NLMSG_ERROR)
		return NL_OK;

	auto e = reinterpret_cast<const struct nlmsgerr *>(nlmsg_data(hdr));
	auto elen = NLMSG_ALIGN(sizeof(*e)) + NLMSG_ALIGN(e->msg.nlmsg_len - NLMSG_HDRLEN);

	std::cout << __func__ << ": error=" << e->error << '\n';

	auto errMsg = nla_find(nlmsg_attrdata(hdr, elen), nlmsg_attrlen(hdr, elen),
			       NLMSGERR_ATTR_MSG);
	if (!errMsg)
		return NL_OK;

	std::cout << "\textack msg: " << reinterpret_cast<const char *>(nla_data(errMsg)) << '\n';

	return NL_OK;
}

int Route::errorCB(struct sockaddr_nl *, struct nlmsgerr *e, void *)
{
	std::cout << __func__ << ": err=" << e->error << '\n';

	return NL_OK;
}

static SockHolder initSock(int protocol)
{
	SockHolder sock = ::nl_socket_alloc();
	if (!sock) {
		std::cerr << "Failed to allocate netlink socket.\n";
		return nullptr;
	}

	auto err = ::nl_connect(sock, protocol);
	if (err < 0) {
		std::cerr << "Failed to connect to netlink: " << ::nl_geterror(err) << '\n';
		return nullptr;
	}

	auto fd = nl_socket_get_fd(sock);

	unsigned int val = 1;
	err = setsockopt(fd, SOL_NETLINK, NETLINK_GET_STRICT_CHK, &val, sizeof(val));
	if (err) {
		std::cerr << "setsockopt(NETLINK_GET_STRICT_CHK): " << strerror(errno) << '\n';
		return nullptr;
	}

	err = setsockopt(fd, SOL_NETLINK, NETLINK_EXT_ACK, &val, sizeof(val));
	if (err) {
		std::cerr << "setsockopt(NETLINK_EXT_ACK): " << strerror(errno) << '\n';
		return nullptr;
	}

	return sock;
}

int Route::run(bool fail)
{
	auto sock = initSock(NETLINK_ROUTE);
	if (!sock)
		return -1;

	CBHolder orig_cb = nl_socket_get_cb(sock);
	CBHolder cb = nl_cb_clone(orig_cb);
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, validCB, nullptr);
	nl_cb_set(cb, NL_CB_MSG_IN, NL_CB_CUSTOM, inCB, nullptr);
	nl_cb_err(cb, NL_CB_CUSTOM, errorCB, nullptr);
	nl_socket_set_cb(sock, cb);

	MsgHolder msg(nlmsg_alloc_simple(RTM_GETLINK, 0));
	if (!msg) {
		std::cerr << "Unable to alloc simple\n";
		return -1;
	}

	struct ifinfomsg ifi = {};
	ifi.ifi_index = 1;
	if (fail)
		ifi.__ifi_pad = 4; // make it wrong -> get extack message
	auto err = nlmsg_append(msg, &ifi, sizeof(ifi), NLMSG_ALIGNTO);
	if (err < 0) {
		std::cerr << "Failed to put: " << ::nl_geterror(err) << '\n';
		return -1;
	}

	__u32 vf_mask = RTEXT_FILTER_VF;
	err = nla_put(msg, IFLA_EXT_MASK, sizeof(vf_mask), &vf_mask);
	if (err < 0) {
		std::cerr << "Failed to put: " << ::nl_geterror(err) << '\n';
		return -1;
	}

	err = nl_send_auto(sock, msg);
	if (err < 0) {
		std::cerr << "Failed to send: " << ::nl_geterror(err) << '\n';
		return -1;
	}

	err = nl_wait_for_ack(sock);
	if (err < 0) {
		std::cerr << "Failed to wait: " << ::nl_geterror(err) << '\n';
		return -1;
	}

	return 0;
}

class Genl {
public:
	static int validCB(struct nl_msg *msg, void *);
	static int inCB(struct nl_msg *msg, void *d) {
		return Route::inCB(msg, d);
	}
	static int errorCB(struct sockaddr_nl *nl, struct nlmsgerr *e, void *d) {
		return Route::errorCB(nl, e, d);
	}

	static int getFamilyId(SockHolder &sock, const std::string &familyName);

	static int run();
};

int Genl::getFamilyId(SockHolder &sock, const std::string &familyName)
{
	struct nl_cache *__cache;
	auto err = genl_ctrl_alloc_cache(sock, &__cache);
	if (err < 0) {
		std::cerr << "Failed to alloc ctrl cache: " << ::nl_geterror(err) << '\n';
		return -1;
	}
	CacheHolder cache(__cache);

	FamilyHolder family(genl_ctrl_search_by_name(cache, familyName.c_str()));
	if (!family) {
		std::cerr << "Unable to find ovpn family\n";
		return -1;
	}

	return genl_family_get_id(family);
}

int Genl::run()
{
	auto sock = initSock(NETLINK_GENERIC);
	if (!sock)
		return -1;

	auto family = getFamilyId(sock, "ovpn");
	if (family < 0)
		return -1;

	CBHolder orig_cb = nl_socket_get_cb(sock);
	CBHolder cb = nl_cb_clone(orig_cb);
	//nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, validCB, nullptr);
	nl_cb_set(cb, NL_CB_MSG_IN, NL_CB_CUSTOM, inCB, nullptr);
	nl_cb_err(cb, NL_CB_CUSTOM, errorCB, nullptr);
	nl_socket_set_cb(sock, cb);

	MsgHolder msg(nlmsg_alloc_simple(OVPN_CMD_PEER_NEW, 0));
	if (!msg) {
		std::cerr << "Unable to alloc simple\n";
		return -1;
	}

	auto userLen = 0U;
	auto userPtr = genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, family, userLen, 0,
				   OVPN_CMD_PEER_NEW,
				   OVPN_FAMILY_VERSION);
	if (!userPtr) {
		std::cerr << "Failed to put\n";
		return -1;
	}

	std::cout << "1: " << nlmsg_hdr(msg)->nlmsg_len << '\n';

	__u32 val = 1;
	auto err = nla_put(msg, OVPN_A_IFINDEX, sizeof(val), &val);
	if (err < 0) {
		std::cerr << "Failed to put: " << ::nl_geterror(err) << '\n';
		return -1;
	}
	std::cout << "2: " << nlmsg_hdr(msg)->nlmsg_len << '\n';

	err = nl_send_auto(sock, msg);
	if (err < 0) {
		std::cerr << "Failed to send: " << ::nl_geterror(err) << '\n';
		return -1;
	}

	err = nl_wait_for_ack(sock);
	if (err < 0) {
		std::cerr << "Failed to wait: " << ::nl_geterror(err) << '\n';
		return -1;
	}

	return 0;
}

int main(int argc, char **)
{
	Clr(Clr::GREEN) << "=== Route ===";
	if (Route::run(argc > 1))
		return EXIT_FAILURE;

	Clr(Clr::GREEN) << "=== Genl ===";
	if (Genl::run())
		return EXIT_FAILURE;

	return 0;
}
