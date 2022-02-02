# 基础
  - 判断点p是否在三角形abc内:

    根据叉乘判断p是否在$\overrightarrow{ab}$, $\overrightarrow{bc}$, $\overrightarrow{ca}$同侧判断点是否在三角形内部。

    若 $\overrightarrow{a}$ 是( $x_{0}$,$y_{0}$), $\overrightarrow{b}$ 是($x_{1}$,$y_{1}$), 则
    $$
    \overrightarrow{a} \times \overrightarrow{b} = (x_{0}y_{1}-x_{1}y_{0})
    $$

    当 $\overrightarrow{ab}$ $\times$ $\overrightarrow{pa}$, $\overrightarrow{bc}$ $\times$ $\overrightarrow{pb}$, $\overrightarrow{ca}$ $\times$ $\overrightarrow{pc}$ 符号相同时,点在三角形内部。


  - 执行三角形栅格化
    - 生成三角形包围盒:
      找到最大, 最小的x, y坐标max_x, min_x, max_y, min_y
    - 遍历包围盒中的每一个像素点, 若该点在三角形内,则计算出该点的插值深度值,若该值小于z-buffer中的深度,则更新像素点的颜色及z-buffer中的深度值。



# SSAA
  - SSAA为将当前分辨率成倍提高, 然后再将图像缩小到显示屏上。
  - 对于作业2来说：
    - 通过创建frame_buf_SSAA和depth_buf_SSAA(其容量为frame_buf和depth_buf的倍数), 将屏幕像素分成n次方个。分别计算每个小像素的颜色值,并求其均值作为原像素的值。

# MSAA
  - MSAA为在光栅化阶段，判断一个三角形是否被像素覆盖的时候会计算多个覆盖样本（Coverage sample），但是在pixel shader着色阶段计算像素颜色的时候每个像素还是只计算一次。
  - 通过创建depth_buf_MASS,判断有多少个采样点在像素内。若m个采样点中有n个在像素内，则该像素颜色为:
  $$
  color_{pixel}=\frac{color_{triangle} \times n}{m}+\frac{color_{old\_pixel}\times(m-n)}{m}
  $$