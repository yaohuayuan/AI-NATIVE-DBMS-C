# include 目录说明

`include/` 只存放公开头文件。

当前 `include/` 下只有 `aidb/` 一个目录是正常的。`aidb/` 是项目级命名空间目录，用于避免与系统头文件和第三方库冲突。

外部代码使用公开 API 时应写：

```c
#include "aidb/aidb.h"
```

内部实现头文件应该放在 `src` 对应模块目录中。不要把 `*_internal.h` 暴露到 `include/`。

当前阶段公开 API 还很少，不要为了看起来丰富而提前暴露内部头文件。未来如果公开头文件变多，也应在 `include/aidb/` 下继续组织，而不是在 `include/` 下创建 `core`、`dbms`、`ai`、`json` 等平级目录。
