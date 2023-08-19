#	Addresses and references to them

| Name               | Address    | Length (sectors) | References                                  |
| :----------------- | :--------- | :--------------- | :------------------------------------------ |
| `setup`            | 0x7C00     | 1                | -                                           |
| `rm-srvldr`        | 0x1000     | 2                | `setup/setup.asm`, `library/gcc/rm-srv.asm` |
| `bootldr`          | 0x2000     | ?                | `setup/setup.asm`, `build/bootldr.ld`       |