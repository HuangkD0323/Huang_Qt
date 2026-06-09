# Qt Excel 客户端示例（QXlsx 版本）

这个仓库现在使用 QXlsx 来读取 .xlsx 文件，因此无需在系统中安装 Microsoft Excel。也同时保留了 CSV 导入作为回退。

主要变更：
- 移除对 ActiveQt（axcontainer）的依赖，改为优先使用 QXlsx（若 third_party/QXlsx 存在则自动包含其源码）。
- 提供脚本 scripts/fetch_qtxlsx.* 来自动把 QXlsx 克隆到 third_party/QXlsx，使得项目开箱即可编译读取 .xlsx。
- 提供 samples/sample_data.csv 作为测试数据。

如何获取 QXlsx（最简单）：
1. 切到 use-qtxlsx 分支或在 master 运行脚本（项目根）：
   - Windows: scripts\fetch_qtxlsx.bat
   - Linux/macOS/WSL/Git Bash: sh scripts/fetch_qtxlsx.sh

2. qmake 会检测 third_party/QXlsx/xlsx 并自动把 QXlsx 的 .cpp/.h 加入构建（ExcelClient.pro 已配置）。

开发/构建：
1. 打开 ExcelClient.pro（Qt Creator -> File -> Open File or Project）。
2. Run qmake -> Build。
3. 点击 “导入 Excel/CSV” 选择 .xlsx（若已抓取 QXlsx）或 .csv（回退）。

License 与来源：
- QXlsx 来自 https://github.com/QtExcel/QXlsx，请保留其 LICENSE 文件（脚本会把 upstream 的文件包含在 third_party/QXlsx）。
