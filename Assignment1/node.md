对于点
$$
\left[
  \begin{matrix}
   x\\
   y\\
   z\\
   1
  \end{matrix}
\right]
$$

- 旋转
  - X
    $$
    \left[
      \begin{matrix}
       x\\
       y\\
       z\\
       1
      \end{matrix}
    \right]
      *
    \left[
      \begin{matrix}
       1& 0& 0& 0\\
       0& cosα& -sinα& 0\\
       0& sinα& cosα& 0\\
       0& 0& 0& 1\\
      \end{matrix}
    \right]
    $$

  - Y
    $$
    \left[
      \begin{matrix}
       x\\
       y\\
       z\\
       1
      \end{matrix}
    \right]
      *
    \left[
      \begin{matrix}
       cosα& 0& sinα& 0\\
       0& 1& 0& 0\\
       -sinα& 0& cosα& 0\\
       0& 0& 0& 1\\
      \end{matrix}
    \right]
    $$

  - Z
    $$
    \left[
      \begin{matrix}
       x\\
       y\\
       z\\
       1
      \end{matrix}
    \right]
      *
    \left[
      \begin{matrix}
       cosα& -sinα& 0& 0\\
       sinα& cosα& 0& 0\\
       0& 0& 1& 0\\
       0& 0& 0& 1\\
      \end{matrix}
    \right]
    $$

  - 绕任意轴

    根据罗德里格旋转公式（Rodrigues' rotation formula）

    轴的单位向量$\overrightarrow{K}$为:
    $$
    \left[
      \begin{matrix}
       a\\
       b\\
       c
      \end{matrix}
    \right]
    $$
    旋转角度为$\theta$;

    矩阵Rk为:
    $$
    \left[
      \begin{matrix}
        0& -c& b\\
        c& 0& -a\\
        -b& a& 0
      \end{matrix}
    \right]
    $$
    modle矩阵为:
    $$
    I + (1-cos(\theta)) \cdot Rk^2+Rk \cdot sin(\theta)
    $$
    因为点使用齐次坐标,所以将modle矩阵变为4x4矩阵

    所以最后可得:
    $$
    \left[
      \begin{matrix}
        x\\
        y\\
        z\\
        1
      \end{matrix}
    \right]
    *
    \left[
      \begin{matrix}
        m_{(0,0)}& m_{(0,1)}& m_{(0,2)}& 0\\
        m_{(1,0)}& m_{(1,1)}& m_{(1,2)}& 0\\
        m_{(2,0)}& m_{(2,1)}& m_{(2,2)}& 0\\
        0& 0& 0& 1
      \end{matrix}
    \right]
    $$