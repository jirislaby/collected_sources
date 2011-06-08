extern void *ptr;

void *get_current(void)
{
	void *ret;

	asm("stosq");
	asm("movq %" "P" "1" ",%0" : "=r" (ret) : "p" (&ptr));
	asm volatile("movq %P1,%0" : "=r" (ret) : "p" (&ptr));

	return ret;
}
