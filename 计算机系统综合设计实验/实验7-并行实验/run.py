from components.boards import HW5X86Board
from components.memories import HW5DDR4
from components.processors import HW5O3CPU
from components.cache_hierarchies import HW5MESITwoLevelCacheHierarchy

from workloads.roi_manager import exit_event_handler
from workloads.array_sum_workload import NaiveArraySumWorkload, ChunkingArraySumWorkload, NoResultRaceArraySumWorkload, ChunkingNoResultRaceArraySumWorkload, NoCacheBlockRaceArraySumWorkload, ChunkingNoBlockRaceArraySumWorkload

from gem5.simulate.simulator import Simulator


if __name__ == "__m5_main__":
  num_cores = 16
  xbar_latency = 10
  cpu = HW5O3CPU(num_cores)
  cache = HW5MESITwoLevelCacheHierarchy(xbar_latency)
  memory = HW5DDR4()
  board = HW5X86Board(clk_freq="3GHz", processor=cpu, cache_hierarchy=cache, memory=memory)
  array_size = 32768
  num_threads = 1
  workload = NaiveArraySumWorkload(array_size, num_threads)
  # workload = ChunkingArraySumWorkload(array_size, num_threads)
  # workload = NoResultRaceArraySumWorkload(array_size, num_threads)
  # workload = ChunkingNoResultRaceArraySumWorkload(array_size, num_threads)
  # workload = NoCacheBlockRaceArraySumWorkload(array_size, num_threads)
  # workload = ChunkingNoBlockRaceArraySumWorkload(array_size, num_threads)
  board.set_workload(workload)
  simulator = Simulator(board=board, full_system=False, on_exit_event=exit_event_handler)
  simulator.run()
  print("Finished simulation.")
