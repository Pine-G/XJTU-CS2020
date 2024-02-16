from components.boards import HW4RISCVBoard
from components.memories import HW4DDR4
from components.processors import HW4O3CPU
from components.cache_hierarchies import HW4SmallCache, HW4MediumCache, HW4LargeCache

from workloads.roi_manager import exit_event_handler
from workloads.matmul_workload import IJKMatMulWorkload, IKJMatMulWorkload, BlockIJMatMulWorkload, BlockIKMatMulWorkload, BlockKJMatMulWorkload

from gem5.simulate.simulator import Simulator


if __name__ == "__m5_main__":
  cpu = HW4O3CPU()
  memory = HW4DDR4()
  # cache = HW4SmallCache()
  # cache = HW4MediumCache()
  cache = HW4LargeCache()
  board = HW4RISCVBoard(clk_freq="2GHz", processor=cpu, cache_hierarchy=cache, memory=memory)
  matrix_size = 128
  block_size = 8
  # workload = IJKMatMulWorkload(matrix_size)
  # workload = IKJMatMulWorkload(matrix_size)
  workload = BlockIJMatMulWorkload(matrix_size, block_size)
  # workload = BlockIKMatMulWorkload(matrix_size, block_size)
  # workload = BlockKJMatMulWorkload(matrix_size, block_size)
  board.set_workload(workload)
  simulator = Simulator(board=board, full_system=False, on_exit_event=exit_event_handler)
  simulator.run()
  print("Finished simulation.")
