from mpi4py import MPI
from app import MyApp, MySlave
from utils import timeit


def main():
    #
    # gets the process Id
    #
    rank = MPI.COMM_WORLD.Get_rank()

    #
    # gets the total count of managed processes
    #
    size = MPI.COMM_WORLD.Get_size()

    #
    # assignes a role to the process based on its Id
    #
    role = "Master" if rank == 0 else f"Slave-{rank}"

    print(f"{role} started (total {size})")

    #
    # 0. Master routine
    #
    if rank == 0:
        app = MyApp(slaves=range(1, size))
        app.run()
        app.terminate_slaves()
    #
    # 1-. Slaves routine
    #
    else:
        MySlave().run()


if __name__ == "__main__":
    elapsed = timeit(main)
    print(f"elapsed: {round(elapsed, 3)}")
