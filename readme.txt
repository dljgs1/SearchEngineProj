搜索引擎，开发中。
目录说明：
core\ :含核心类：倒排表Cinverted_table、词项term、表项record_table、分析器Canalysis（开发中）
format\:含格式化类：编码映射矩阵类gb2312_mat、编码分割类gb2312_str、汉字拼音转换类charspell
io\:含输入输出类：汉字读取器Creader、过滤器Cfilter
mem\:含类数据管理类Cclass_data、数据管理模板Tdata_man、以及调试界面（暂用）
debugtool\:调试工具（暂无用）

当前支持功能：对字、词进行严格检索。
目前任务：分析器进行数据挖掘，使用拼音转换进行补正搜索，使用通配符进行模糊搜索。

use github to work together
