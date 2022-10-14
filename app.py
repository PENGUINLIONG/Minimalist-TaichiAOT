import numpy as np
import taichi as ti

ti.init(arch=ti.vulkan)

@ti.kernel
def taichi_add(in_ten: ti.types.ndarray(), out_ten: ti.types.ndarray()):
    for i, j, k in in_ten:
        out_ten[i, j, k] = in_ten[i, j, k] / 7.0 + in_ten[i, j, k] * 0.3 + 1.0

in_ten   = ti.ndarray(dtype=ti.f32, shape=(320, 320, 4))
out_ten = ti.ndarray(dtype=ti.f32, shape=(320, 320, 4))

arg_0 = ti.graph.Arg(ti.graph.ArgKind.NDARRAY, "in_ten", dtype=ti.f32, field_dim=3)
arg_1 = ti.graph.Arg(ti.graph.ArgKind.NDARRAY, "out_ten", dtype=ti.f32, field_dim=3)
g = ti.graph.GraphBuilder()
g.dispatch(taichi_add, arg_0, arg_1)
g = g.compile()

m = ti.aot.Module(ti.vulkan)
m.add_graph("taichi_add", g)
m.save("module", "")