#include <rpc/rpc.h>
#include <err.h>

#define RUSERSPROG 0x40000000
#define RUSERSVERS 1

int main(void)
{
	CLIENT *cln;
	struct timeval tout = { 1, 0 };

	cln = clnt_create("localhost", RUSERSPROG, RUSERSVERS, "tcp");
	if (cln == NULL)
		err(1, "clnt_create");

	puts("xxx");
	clnt_call(cln, 0, (xdrproc_t)xdr_void, NULL, (xdrproc_t)xdr_void, NULL, tout);
	puts("xxx");
	clnt_call(cln, 1, (xdrproc_t)xdr_void, NULL, (xdrproc_t)xdr_void, NULL, tout);
	puts("xxx");

	clnt_destroy(cln);

	return 0;
}
