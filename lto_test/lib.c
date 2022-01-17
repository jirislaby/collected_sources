#include <errno.h>
#include <stdio.h>

#include <pciaccess.h>

#include "lib.h"

static struct pci_device *pdev;
static uint32_t *volatile bar0_space;

int lib_prep(void)
{
	static const struct pci_id_match id = {
		0x1234, 0x11e8, PCI_MATCH_ANY, PCI_MATCH_ANY,
	};
	struct pci_device_iterator *it;
	struct pci_mem_region *bar0;
	void *mem;
	int ret;

	ret = pci_system_init();
	if (ret)
		goto errno_ret;

	it = pci_id_match_iterator_create(&id);
	if (!it) {
		ret = EIO;
		goto err_cleanup;
	}

	pdev = pci_device_next(it);
	pci_iterator_destroy(it);

	if (!pdev) {
		ret = ENODEV;
		goto err_cleanup;
	}

	printf("Found %.4x:%.2x:%.2x.%x\n", pdev->domain_16, pdev->bus, pdev->dev,
			pdev->func);

	ret = pci_device_probe(pdev);
	if (ret)
		goto err_cleanup;

	bar0 = &pdev->regions[0];
	printf("bar0: 0x%lx (size=0x%lx)\n", bar0->base_addr, bar0->size);

	ret = pci_device_map_range(pdev, bar0->base_addr, bar0->size,
			PCI_DEV_MAP_FLAG_WRITABLE, &mem);
	if (ret)
		goto err_cleanup;

	bar0_space = mem;

	return 0;
err_cleanup:
	pci_system_cleanup();
errno_ret:
	errno = ret;
	return -1;
}

void lib_fun(unsigned int a)
{
	static uint32_t last = ~0 - 1;
	uint32_t c;

	c = bar0_space[1];
	bar0_space[1] = ~c;
	if (c != last) {
		printf("%u: 0x%.8x\n", a, c);
		if (c != last)
			last = c;
	}
}

void lib_exit(void)
{
	pci_device_unmap_range(pdev, (void *)bar0_space, pdev->regions[0].size);
	pci_system_cleanup();
}
