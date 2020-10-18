## linux kernel repository

### linux-surface branches

`v${kernelver}-surface-devel`:
- tracking upstream [linux-surface/kernel](linux-surface/kernel) development branches, if exist in my repo.

`v${kernelver}-surface`:
- tracking upstream [linux-surface/kernel](linux-surface/kernel) patch generation branches, if exist in my repo. Patches will be created by cherry-picking commits from `-devel` branches and squashing.

### chromiumos kernel branches

Release branches:
- [release/chromeos\-4.19+surface+k5](https://github.com/kitakar5525/linux-kernel/tree/release/chromeos-4.19%2Bsurface%2Bk5)
- [release/chromeos\-5.4+surface+k5](https://github.com/kitakar5525/linux-kernel/tree/release/chromeos-5.4%2Bsurface%2Bk5)

v5.4 devel branches:
- [chromeos\-5.4\-surface\-devel+k5](https://github.com/kitakar5525/linux-kernel/tree/chromeos-5.4-surface-devel%2Bk5)
- [chromeos\-5.4\-surface\-devel\-uapi\_ipts](https://github.com/kitakar5525/linux-kernel/tree/chromeos-5.4-surface-devel-uapi_ipts)
- [chromeos\-5.4\-surface\-devel](https://github.com/kitakar5525/linux-kernel/tree/chromeos-5.4-surface-devel)

v4.19 devel branches:
- [chromeos\-4.19\-surface\-devel+k5](https://github.com/kitakar5525/linux-kernel/tree/chromeos-4.19-surface-devel%2Bk5)
- [chromeos\-4.19\-surface\-devel](https://github.com/kitakar5525/linux-kernel/tree/chromeos-4.19-surface-devel)

### Cherry Trail related branches (mainly for atomisp stuff leaning)

Android-IA kernel trees for Cherry Trail, applied kernel quilts from Intel:
- [ProductionKernelQuilts/cht-m1stable-2016_ww31](https://github.com/kitakar5525/linux-kernel/tree/ProductionKernelQuilts/cht-m1stable-2016_ww31)
- [ProductionKernelQuilts/cht-m1stable-2016_ww19](https://github.com/kitakar5525/linux-kernel/tree/ProductionKernelQuilts/cht-m1stable-2016_ww19)

Android-IA vendor kernel trees:
- [lenovo_yb1_x90f_l_osc_201803](https://github.com/kitakar5525/linux-kernel/tree/lenovo_yb1_x90f_l_osc_201803)
  [Based on v3.14.55]
- [xiaomi-mipad2/latte-l-oss](https://github.com/kitakar5525/linux-kernel/tree/xiaomi-mipad2/latte-l-oss)
  [Based on v3.14.37]

### Bay Trail related branches (mainly for atomisp stuff leaning)

Android-IA vendor kernel trees:
- [lenovo_yoga_tab_2_osc_android_to_lollipop_201505](https://github.com/kitakar5525/linux-kernel/tree/lenovo_yoga_tab_2_osc_android_to_lollipop_201505)
  [Based on v3.10.20. Contains ov8865 sensor driver for atomisp.]
