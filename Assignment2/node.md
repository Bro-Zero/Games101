# 基础
  - 判断点p是否在三角形abc内:

    根据叉乘判断p是否在$\overrightarrow{ab}$, $\overrightarrow{bc}$, $\overrightarrow{ca}$同侧判断点是否在三角形内部。

    若 $\overrightarrow{a}$ 是( $x_{0}$,$y_{0}$), $\overrightarrow{b}$ 是($x_{1}$,$y_{1}$), 则
    $$
    \overrightarrow{a} \times \overrightarrow{b} = (x_{0}y_{1}-x_{1}y_{0})
    $$

    当 $\overrightarrow{ab}$ x $\overrightarrow{pa}$, $\overrightarrow{bc}$ x $\overrightarrow{pb}$, $\overrightarrow{ca}$ x $\overrightarrow{pc}$ 符号相同时,点在三角形内部。


  - 执行三角形栅格化
    - 生成三角形包围盒:
      找到最大, 最小的x, y坐标max_x, min_x, max_y, min_y
    - 遍历包围盒中的每一个像素点, 若该点在三角形内,则计算出该点的插值深度值,若该值小于z-buffer中的深度,则更新像素点的颜色及z-buffer中的深度值。



# SSAA
  - SSAA为将当前分辨率成倍提高, 然后再将图像缩小到显示屏上。
  - 对于作业2来说：
    - 通过创建frame_buf_SSAA和depth_buf_SSAA(其容量为frame_buf和depth_buf的倍数), 将屏幕像素分成n次方个。分别计算每个小像素的颜色值,并求其均值作为原像素的值。