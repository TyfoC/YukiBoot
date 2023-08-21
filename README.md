# YukiBoot - bootloader for x86
##	**Be sure to check out the kernel build example (`kernel-example/build/Makefile`) !**
##	**Also don't forget to look at the sample configuration file (`storage/boot.cfg`) !**
###	How can I boot my kernel?
####	You should put it in the `storage` folder, set kernel boot options in `boot.cfg` file

###	How to configure `boot.cfg`?
####	You need to set the path to the kernel, the kernel file format (only `YBT` is supported at the moment (link the kernel using `kernel-example/build/ybt.ld`)), settings for the required video mode
####	There are currently three modes:
| Video mode type | Width | Height | Bits/Pixel | VESA mode attributes | VESA mem model type |
| :-------------- | :---: | :----: | :--------: | :------------------: | :-----------------: |
| `VESA_GRAPHICS` | `UD`  | `UD`   | `UD`       | `UD`                 | `UD`                |
| `VGA_TEXT `     | 80    | 25     | 8          | -                    | -                   |
| `VGA_GRAPHICS`  | 320   | 200    | 8          | -                    | -                   |

#####	Note:	`UD` - user-defined
####	The `VGA_TEXT` and `VGA_GRAPHICS` modes have standard video buffer addresses: `0xB8000` and `0xA0000`.
####	The kernel receives all collected information about the machine in the form of a `SystemInfoBlock_t` structure.