# Qt Excel 客户端示例（ActiveQt 版本）

这个仓库包含一个使用 Qt（C++）和 ActiveQt (QAxObject) 实现的最小客户端示例，能在 Windows 10 上读取本机安装的 Microsoft Excel 文件（.xls/.xlsx），并在界面中显示、检索、增加、删除行。

文件列表：
- ExcelClient.pro
- main.cpp
- mainwindow.h
- mainwindow.cpp
- filterproxymodel.h

先决条件：
- Windows 10
- Qt（建议 Qt 5.12+ 或 Qt6），请确保在安装组件中包含 ActiveQt（axcontainer）模块
- 已安装 Microsoft Excel（QAxObject 依赖 COM Excel）

构建与运行：
1. 在 Qt Creator 中打开项目：File -> Open File or Project -> 选择仓库根目录下的 ExcelClient.pro。
2. 选择合适的 Kit（MSVC 或 MinGW），然后构建并运行。

使用说明：
- 点击 “导入 Excel” 选择 .xlsx/.xls 文件，默认读取第一个工作表。
- 第一行将被视作表头，余下行为数据并显示在表格中。
- 在右上角输入关键字可进行跨列检索（不区分大小写）。
- 点击 “新增行” 添加可编辑的新行。双击单元格编辑内容。
- 选择行后点击 “删除选中” 可删除选中行。

注意事项与扩展：
- 如果你的机器没有安装 Microsoft Excel，我可以把读取逻辑改为使用 QtXlsx 库（无需 Excel）。如果你需要，请回复我，我会把代码改为 QtXlsx 版本并再次提交。
- 当前实现将整个表格加载到 QStandardItemModel，若数据量很大（数万行），可考虑分页或使用更高效的模型实现。

如果你需要我：
- 改成 QtXlsx（无需 Excel），请回复“使用 QtXlsx”。
- 添加“保存到 Excel”或导出为 CSV 功能，请告诉我具体需求。
