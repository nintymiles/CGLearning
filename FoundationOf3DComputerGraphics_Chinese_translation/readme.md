# Note
这本书正在不断的学习中，每读一遍都受益匪浅。当对一种文献拥有足够的熟悉度，那么翻译出来自然会有一定的准确度。这本教材适于没有专业的数学和物理知识的人员使用。内容深浅有度，可浅可深。但是所提供习题样例却又具有相当的深度。需要不断的理解书中内容，逐步的解决。

## 翻译过程中所用到的一些Markdown语法

### Markdown一般语法的注意事项
1. Section（#）和List（*）等标记和内容之间要留下空格，否则github不能正常解析。
2. 加粗\*\*粗体内容\*\*，粗体内容开始结尾处不能有空格，否则不能正常解析
3. 清单条目可以包含多个段落，但是每个段落间要有空行隔开，并且新的段落要以tab符开头 

### 使用到的一些Markdown MathJax表达
注意这里不是标准的Markdown语法而是用于数学公式表达的语法。

#### 1. 基本格式控制

| 内容 | 格式 |
| --- | --- |
| 空格表达 |四种宽度: \\,  、 \\;  、 \\quad  、 \\qquad |
| 注释表达，当然注释中使用空格字符串也可插入空格(但是只有一个空格生效) | \\text{Figure 19.7} 、\\text{ } |
| 在数学公式中的粗体表示 | \\mathbf{c},注意基本格式控制要用在数学符号控制的外围，而非其内。例如：\\mathbf{\vec{c}} |
| 控制字体缩放 | \\normalsize \\small \\Small \\SMALL \\large \\Large \\LARGE \huge \Huge \HUGE{公式内容} ，其中大些字母越多代表相应行为的程度越大|
| 单行公式表达“\$公式\$”注意实现 | 在有些实现中，单行表达时，不能有前后空格，否则解析错误 |
| 公式的多行表达 | 若公式很复杂，或者为了可读性美观性想换行表达，则使用双`$$` 围住多行公式，但要注意不要有空白行（可以理解为公式表达内容字符中间允许多个非连续回车符），否则会出现解析错误 |
| 公式的换行表达 | 在公式表达中欲换行解析的地方添加 `\\` |
| Copyright符号 | 直接使用字符 ©️,目前的markdown并不支持latex的`\copyright`表达 | --- |


#### 2. 常用数学符号表达

| 内容 | 格式 |  样例  |
| --- | --- | --- |
| 矢量的表达 | `$\vec{x}$`|  $\quad \vec{c} \quad 、 \mathbf{\vec{c}}$  |
| 用到的拉丁字母 | `$\alpha$ 、$\lambda$ 、$\Omega$` | $\alpha 、 \lambda 、 \Omega$ |
| 上下标 | 使用\^和\_，如`$x\i$ ,x^2$`;若上下标长度超过1，将内容放入{}，例如 `$x_{123}$` | $C^2 、 C_m 、 C_{123m}$  |
| 分数形式表达 | `$\frac{分子}{分母}$` | $\frac{1}{.45}$ |
| 矩阵表达 |`$\begin{[p/b/B/v/V]matrix} row1a&row1b&row1c \\ row2a&row2b&row2c \\ row3a&row3b&row3c \end{[p/b/B/v/V]matrix}$`,若matrix之前没有字母则生成的矩阵没有包围符号，若有，则p表示包围符号(),b表示包围符号[],B表示包围符号{},V表示包围符号"∥∥" | $\begin{Vmatrix} row1a&&row1b&&row1c \\ row2a&&row2b&&row2c \\ row3a&&row3b&&row3c \end{Vmatrix}$ |
| 求和表达式 | `$\sum_i^ja_ib_i$`,如果想让i下标在求和符号的下方，请使用`$$\sum_ia_ib_i$$`方式表达 | $$\sum_ia_i\vec{b}_i$$|

#### 3. 复杂数学公式（方程式）的对齐排列以及特殊位置分隔符的设置
翻译过程中，经常会碰到一些多行数学公式的排版十分复杂，要求不同的对齐方式，还会要求一些特殊的分隔符。

`array`函数可以十分完美的完成这个任务。实际上matrix可以看作是array的特例。在array中可以针对每一列公式设置对齐方式，并添加分隔符。

比如一个表达有三列内容，针对每一列设置一种对齐方式，并在第二列后添加一个竖线分隔符，这样写`$$\begin{array}{cl|r} ... & content & ... \end{array}$$`。其中c代表居中对齐，l代表做对齐，r代表右对齐，`|`代表竖线分隔符。

从中可见array函数的排版功能十分强大，这里只是一角。

下面举一个真实例子并且展示其效果

```latex
$$ 
\begin{array}{rrl}   
\vec{\mathbf{b}}^t\mathbf{c}.\vec{\mathbf{b}}^t\mathbf{d} & \mbox{=} 
	& (\sum_i \vec{b}_ic_i).(\sum_j \vec{b}_jd_j) \\
& \mbox{=} & \sum_i \sum_j c_id_j(\vec{b}_i.\vec{b}_j) \\
& \mbox{=} & \sum_i c_id_i
\end{array} 
$$
```

$$\large{\begin{array}{rrl} 
\vec{\mathbf{b}}^t\mathbf{c}.\vec{\mathbf{b}}^t\mathbf{d} & \mbox{=} 
& (\sum_i\vec{b}_ic_i).(\sum_j\vec{b}_jd_j) \\
& \mbox{=} & \sum_i\sum_jc_id_j(\vec{b}_i.\vec{b}_j) \\
& \mbox{=} & \sum_ic_id_i
\end{array}}$$










