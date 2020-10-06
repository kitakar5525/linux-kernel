/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Header file for PCIe ASPM control.
 */

void mwifiex_disable_aspm_l0s(struct pci_dev *pdev);
void mwifiex_disable_aspm_l1(struct pci_dev *pdev);
void mwifiex_enable_aspm_l0s(struct pci_dev *pdev);
void mwifiex_enable_aspm_l1(struct pci_dev *pdev);
void mwifiex_enable_aspm_l0s_and_l1(struct pci_dev *pdev);
