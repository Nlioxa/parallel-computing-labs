from mpi4py import MPI
from mpi_master_slave import Master, Slave, WorkQueue
from parse import find_pattern


class MyApp(object):
    """
    This application has lots of work to do so it delegates tasks
    to slaves until everything is done
    """

    def __init__(self, slaves):
        # when creating the Master we tell it what slaves it can handle
        self.master = Master(slaves)
        # WorkQueue is a convenient class that run slaves on a tasks queue
        self.work_queue = WorkQueue(self.master)

    def terminate_slaves(self):
        """
        Call this to make all slaves exit their run loop
        """
        self.master.terminate_slaves()

    def run(self):
        """
        keep starting slaves as long as there is work to do
        """
        workers = self.master.num_slaves()

        text = ""
        with open("text.txt", "r") as f:
            text = f.read()

        #
        # split text into equal parts
        #
        ratio = int(len(text) / workers)

        for i in range(workers):
            beg, end = max(0, ratio * i), min(len(text) - 1, ratio * (i + 1))
            sub_text = text[beg:end]
            self.work_queue.add_work(data=(find_pattern, sub_text, "love"))

        while not self.work_queue.done():
            #
            # give more work to do to each idle slave (if any)
            #
            self.work_queue.do_work()

            #
            # reclaim returned data from completed slaves
            #
            for slave_return_data in self.work_queue.get_completed_work():
                done, data = slave_return_data
                if done:
                    print(
                        f'Master: slave-{data[0]} scanned text and found {data[1] if len(data[1]) else "nothing"}'
                    )


class MySlave(Slave):
    """
    A slave process extends Slave class, overrides the 'do_work' method
    and calls 'Slave.run'. The Master will do the rest
    """

    def __init__(self):
        super(MySlave, self).__init__()

    def do_work(self, data):
        rank = MPI.COMM_WORLD.Get_rank()

        find, text, pattern = data
        indices = find(text, pattern)

        return (True, (rank, indices))
