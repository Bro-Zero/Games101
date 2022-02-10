- blinn-phong模型：
  - 求出光的方向和视角方向和衰减因子r
  - 求环境光分量，这里直接用环境光颜色ka乘以强度即可，因为环境光默认是不会衰减的
  - 求漫反射分量，根据公式求即可(kd为片段颜色)：
  $$
  L_{d} = k_{d}\left(I/r^{2}\right)max(0,\pmb{n}\cdot\pmb{l})
  $$
  - 求镜面反射分量，根据公式求即可：
  $$
  L_{s}=k_{s}\left(I/r^{2}\right)\max(0, cos\alpha)^{p}
  $$
  $$
  L_{s}=k_{s}\left(I/r^{2}\right)\max(0, \pmb{n}\cdot\pmb{h})^{p}
  $$