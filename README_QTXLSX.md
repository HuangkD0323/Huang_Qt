我已把 QXlsx 源码拉取脚本加入仓库，并修改 .pro 以便于开箱即用。

现在仓库包含：
- scripts/fetch_qtxlsx.sh （在类 Unix 环境或 Git Bash 下运行）
- scripts/fetch_qtxlsx.bat （在 Windows CMD 下运行）
- third_party/.gitkeep （占位）

使用步骤（开箱即用）：
1. 切换到 use-qtxlsx 分支并拉取最新代码：
   git checkout use-qtxlsx
   git pull
2. 获取 QXlsx 源码（运行下列其一）：
   - Windows (在仓库根目录双击或运行)： scripts\fetch_qtxlsx.bat
   - Linux/macOS/WSL/Git Bash： sh scripts/fetch_qtxlsx.sh

   这会把 QXlsx 仓库克隆到 third_party/QXlsx。脚本使用 --depth 1 来加快下载。

3. 打开 ExcelClient.pro（Qt Creator -> Open File or Project），然后构建。
   qmake 会检测 third_party/QXlsx/xlsx 并自动把 QXlsx 的 .cpp/.h 加入构建。

注意：
- 如果你习惯使用 submodule 或手动集成，也可以自行把 QXlsx 放到 third_party/QXlsx 路径。
- 我未把 QXlsx 的全部源码直接放入仓库以避免一次性提交过大内容；脚本会为你自动获取源码并使项目开箱即用。如果你需要我把 QXlsx 的源码直接（完整拷贝）提交到仓库以避免外部拉取，请明确告诉我，我会把它作为后续 commit 推送（注意：这会显著增加仓库体积）。
