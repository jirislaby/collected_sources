#include <rpc/rpc.h>
#include <rpc/pmap_clnt.h>
#include <err.h>

#define RUSERSPROG 0x40000000
#define RUSERSVERS 1

static void dispatch(struct svc_req *req, SVCXPRT *xprt)
{
	if (req->rq_proc == 0) {
		int in;
		svc_getargs(xprt, (xdrproc_t)xdr_int, (char *)&in);
		in++;
		svc_sendreply(xprt, (xdrproc_t)xdr_int, (char *)&in);
	}
	printf("request %ld\n", req->rq_proc);
}

int main(void)
{
	SVCXPRT *xprt;
	xprt = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (xprt == NULL)
		errx(2, "svctcp_create\n");
	pmap_unset(RUSERSPROG, RUSERSVERS);
	if (!svc_register(xprt, RUSERSPROG, RUSERSVERS, dispatch, IPPROTO_TCP))
		errx(1, "svc_register\n");
	svc_run();
	return 0;
}
