import taichi as ti
import numpy as np

ti.init(arch=ti.vulkan)

n = 32
canvas = ti.ndarray(dtype=ti.f32, shape=(n * 2, n))


@ti.func
def complex_sqr(z):
    return ti.Vector([z[0]**2 - z[1]**2, z[1] * z[0] * 2])


@ti.kernel
def fractal(t: ti.f32, canvas: ti.types.ndarray(field_dim=2)):
    for i, j in canvas:  # Parallelized over all pixels
        c = ti.Vector([-0.8, ti.cos(t) * 0.2])
        z = ti.Vector([i / n - 1, j / n - 0.5]) * 2
        iterations = 0
        while z.norm() < 20 and iterations < 50:
            z = complex_sqr(z) + c
            iterations += 1
        canvas[i, j] = 1 - iterations * 0.02

sym_t = ti.graph.Arg(ti.graph.ArgKind.SCALAR,
                     "t",
                     ti.f32,
                     element_shape=())
sym_canvas = ti.graph.Arg(ti.graph.ArgKind.NDARRAY,
                          "canvas",
                          ti.f32,
                          field_dim=2,
                          element_shape=())

gb = ti.graph.GraphBuilder()
gb.dispatch(fractal, sym_t, sym_canvas)
graph = gb.compile()

mod = ti.aot.Module(ti.vulkan)
mod.add_graph('fractal', graph)
mod.save("module", "")
