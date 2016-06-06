#include <linux/module.h>
#include <linux/acpi.h>

static void handle_table_phys(const phys_addr_t phys);

static void dump_header(const char *prefix, const phys_addr_t phys,
		const struct acpi_table_header *h)
{
	pr_info("%s%.*s@%pad len=%u, oem_id=%.*s oem_table_id=%.*s asl_compiler_id=%.*s\n",
			prefix,
			ACPI_NAME_SIZE, h->signature, &phys, h->length,
			ACPI_OEM_ID_SIZE, h->oem_id,
			ACPI_OEM_TABLE_ID_SIZE, h->oem_table_id,
			ACPI_NAME_SIZE, h->asl_compiler_id);
}

static void handle_rsdt(const struct acpi_table_header *h)
{
	struct acpi_table_rsdt *r = (struct acpi_table_rsdt *)h;
	unsigned count, a;

	count = (h->length - sizeof(r->header)) / ACPI_RSDT_ENTRY_SIZE;

	pr_info("\t%s: dumping %u entries\n", __func__, count);

	for (a = 0; a < count; a++)
		handle_table_phys(r->table_offset_entry[a]);
}

static void handle_apic(const struct acpi_table_header *h)
{
	const struct acpi_table_madt *a = (const struct acpi_table_madt *)h;
#if 0
	const void *w;
	unsigned c = 0;
#endif

	pr_info("\tA=%.8x F=%.8x rest_size=%zu\n", a->address, a->flags,
			h->length - sizeof(*a));
#if 0
	for (w = a + 1; w < (void *)h + h->length; ) {
		const struct acpi_subtable_header *st = w;

		pr_info("\t\toff=%zu T=%.2d L=%2d\n", w - (void *)h, st->type,
				st->length);

		if (!st->length)
			break;
		w += st->length;
		if (++c == 100) {
			pr_info("wtf? %p from %p\n", w, h);
			break;
		}
	}
#endif
}

static void handle_hpet(const struct acpi_table_header *h)
{
	struct acpi_table_hpet *hp = (struct acpi_table_hpet *)h;
	void __iomem *hpet;

	pr_info("\tvend=%.4x comps=%u base=%u:%.16llx no=%u min_tick=%u\n",
			hp->id >> 16, (hp->id >> 8) & 0x1f,
			hp->address.space_id, hp->address.address,
			hp->sequence, hp->minimum_tick);
	hpet = ioremap(hp->address.address, 1024);
	if (!hpet)
		return;
	pr_info("\tvend=%.4x\n", readl(hpet) >> 16);
	pr_info("\tvend=%.16lx\n", readq(hpet));
	iounmap(hpet);
}

static void handle_table_phys(const phys_addr_t phys)
{
	struct acpi_table_header *h;
	u32 real_len;

	h = ioremap(phys, sizeof(*h));
	if (!h)
		return;
	real_len = h->length;
	iounmap(h);

	h = ioremap(phys, real_len);
	if (!h)
		return;

	dump_header("", phys, h);

	if (!strncmp("RSDT", h->signature, ACPI_NAME_SIZE))
		handle_rsdt(h);
	else if (!strncmp("APIC", h->signature, ACPI_NAME_SIZE))
		handle_apic(h);
	else if (!strncmp("HPET", h->signature, ACPI_NAME_SIZE))
		handle_hpet(h);

	iounmap(h);
}

static void dump_rsdp(const phys_addr_t phys, const struct acpi_table_rsdp *r)
{
	pr_info("%s: RSDP@%pad SIG=%.8s oem_id=%.6s rev=%u rsdt=%.8x len=%u xsdt=%.8llx\n",
			__func__, &phys,
			r->signature, r->oem_id, r->revision,
			r->rsdt_physical_address, r->length,
			r->xsdt_physical_address);

	handle_table_phys(r->rsdt_physical_address);
}

static int my_init(void)
{
	void *a, *virt = ioremap(ACPI_HI_RSDP_WINDOW_BASE,
			ACPI_HI_RSDP_WINDOW_SIZE);

	if (!virt)
		return -EIO;

	for (a = virt; a < virt + ACPI_HI_RSDP_WINDOW_SIZE; a += 16)
		if (!strncmp(a, ACPI_SIG_RSDP, 8))
			dump_rsdp(ACPI_HI_RSDP_WINDOW_BASE + a - virt, a);

	iounmap(virt);

	return -ENOMEM;
}
module_init(my_init);

static void my_exit(void)
{
}
module_exit(my_exit);

MODULE_LICENSE("GPL");
