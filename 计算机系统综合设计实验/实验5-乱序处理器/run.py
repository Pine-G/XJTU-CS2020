from components.boards import HW3RISCVBoard
from components.cache_hierarchies import HW3MESICache
from components.memories import HW3DDR4
from components.processors import HW3BigCore, HW3LittleCore, HW3MediumCore0, HW3MediumCore1, HW3MediumCore2, HW3MediumCore3

from workloads.roi_manager import exit_event_handler
from workloads.matmul_workload import MatMulWorkload
from workloads.bfs_workload import BFSWorkload
from workloads.bubble_sort_workload import BubbleSortWorkload

from gem5.simulate.simulator import Simulator


if __name__ == "__m5_main__":
  # cpu = HW3BigCore()
  # cpu = HW3LittleCore()
  cpu = HW3MediumCore3()
  memory = HW3DDR4()
  cache = HW3MESICache()
  board = HW3RISCVBoard(clk_freq="2GHz", processor=cpu, cache_hierarchy=cache, memory=memory)
  # workload = MatMulWorkload()
  workload = BFSWorkload()
  # workload = BubbleSortWorkload()
  board.set_workload(workload)
  simulator = Simulator(board=board, full_system=False, on_exit_event=exit_event_handler)
  simulator.run()
  score = cpu.get_area_score()
  print(f"score: {score}")
  print("Finished simulation.")
