// SPDX-License-Identifier: GPL-2.0
/*
 * File for PCIe ASPM control.
 */

/* The low-level PCI operations will be performed in this file. Therefore,
 * let's use dev_*() instead of mwifiex_dbg() here to avoid troubles.
 */

#include <linux/pci.h>

void mwifiex_disable_aspm_l0s(struct pci_dev *pdev)
{
	dev_info(&pdev->dev, "disabling ASPM L0s state\n");
	pci_disable_link_state(pdev, PCIE_LINK_STATE_L0S);
}

void mwifiex_disable_aspm_l1(struct pci_dev *pdev)
{
	dev_info(&pdev->dev, "disabling ASPM L1 state\n");
	pci_disable_link_state(pdev, PCIE_LINK_STATE_L1);
}

void mwifiex_enable_aspm_l0s(struct pci_dev *pdev)
{
	struct pci_dev *parent_pdev = pdev->bus->self;

	dev_info(&pdev->dev, "enabling ASPM L0s state\n");

	/* If the driver does not have access to the upstream component,
	 * it cannot support ASPM at all.
	 */
	if (!parent_pdev) {
		dev_err(&pdev->dev,
			"driver does not have access to the upstream component\n");
		return;
	}

	/* Enable first in upstream component and then downstream */
	pcie_capability_clear_and_set_word(parent_pdev, PCI_EXP_LNKCTL,
					   PCI_EXP_LNKCTL_ASPMC,
					   PCI_EXP_LNKCTL_ASPM_L0S);
	pcie_capability_clear_and_set_word(pdev, PCI_EXP_LNKCTL,
					   PCI_EXP_LNKCTL_ASPMC,
					   PCI_EXP_LNKCTL_ASPM_L0S);
}

void mwifiex_enable_aspm_l1(struct pci_dev *pdev)
{
	struct pci_dev *parent_pdev = pdev->bus->self;

	dev_info(&pdev->dev, "enabling ASPM L1 state\n");

	/* If the driver does not have access to the upstream component,
	 * it cannot support ASPM at all.
	 */
	if (!parent_pdev) {
		dev_err(&pdev->dev,
			"driver does not have access to the upstream component\n");
		return;
	}

	/* Enable first in upstream component and then downstream */
	pcie_capability_clear_and_set_word(parent_pdev, PCI_EXP_LNKCTL,
					   PCI_EXP_LNKCTL_ASPMC,
					   PCI_EXP_LNKCTL_ASPM_L1);
	pcie_capability_clear_and_set_word(pdev, PCI_EXP_LNKCTL,
					   PCI_EXP_LNKCTL_ASPMC,
					   PCI_EXP_LNKCTL_ASPM_L1);
}

void mwifiex_enable_aspm_l0s_and_l1(struct pci_dev *pdev)
{
	struct pci_dev *parent_pdev = pdev->bus->self;

	dev_info(&pdev->dev, "enabling ASPM L0s and L1 states\n");

	/* If the driver does not have access to the upstream component,
	 * it cannot support ASPM at all.
	 */
	if (!parent_pdev) {
		dev_err(&pdev->dev,
			"driver does not have access to the upstream component\n");
		return;
	}

	/* Enable first in upstream component and then downstream */
	pcie_capability_clear_and_set_word(parent_pdev, PCI_EXP_LNKCTL,
					   PCI_EXP_LNKCTL_ASPMC,
					   PCI_EXP_LNKCTL_ASPM_L0S |
					   PCI_EXP_LNKCTL_ASPM_L1);
	pcie_capability_clear_and_set_word(pdev, PCI_EXP_LNKCTL,
					   PCI_EXP_LNKCTL_ASPMC,
					   PCI_EXP_LNKCTL_ASPM_L0S |
					   PCI_EXP_LNKCTL_ASPM_L1);
}
