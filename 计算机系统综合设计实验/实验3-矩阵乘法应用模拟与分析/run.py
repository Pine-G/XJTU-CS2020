from components.boards import HW1RISCVBoard
from components.cache_hierarchies import HW1MESITwoLevelCache
from components.processors import HW1TimingSimpleCPU
from components.processors import HW1MinorCPU
from components.memories import HW1DDR3_1600_8x8
from components.memories import HW1DDR3_2133_8x8
from components.memories import HW1LPDDR3_1600_1x32

from workloads.mat_mul_workload import MatMulWorkload

from gem5.simulate.simulator import Simulator


if __name__ == "__m5_main__":
  clk = "4GHz"
  # cpu = HW1TimingSimpleCPU()
  cpu = HW1MinorCPU()
  # memory = HW1DDR3_1600_8x8()
  # memory = HW1DDR3_2133_8x8()
  memory = HW1LPDDR3_1600_1x32()
  cache = HW1MESITwoLevelCache()
  board = HW1RISCVBoard(clk_freq=clk, processor=cpu, cache_hierarchy=cache, memory=memory)
  workload = MatMulWorkload(224)
  board.set_workload(workload)
  simulator = Simulator(board=board, full_system=False)
  simulator.run()
  print("Finished simulation.")
