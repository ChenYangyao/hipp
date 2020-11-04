Simple communication
===============================================================

Point-to-point communication
--------------------------------------------------------------

Here is a simple example of point-to-point communication:::

    using HIPP::MPI::DOUBLE;
    HIPP::MPI::Env env;
    auto world = env.world();
    if (world.rank() == 0) {
        // init data
        std::vector<double> v_send(10);
        for (int i = 0; i < v_send.size(); ++i)
            v_send[i] = i;
        // send data
        world.send(1, 0, v_send.data(), 10, DOUBLE);
    }
    if (world.rank() == 1) {
        // recive data, you should pre-allocate a suffcient large space
        std::vector<double> v_recv(20);
        auto status = world.recv(0, 0, v_recv.data(), 20, DOUBLE);
        // output data, the status contains the information about the received data
        cout << "Number of DOUBLEs received: " << status.count(DOUBLE) << endl;
        for (int i = 0; i < status.count(DOUBLE); ++i)
            cout << v_recv[i] << ", ";
        cout << endl;
    }

Collective communication
--------------------------------------------------------------

The collective communication is also easy to implement. For example, if we want
to calculate 1 + 2 + ... + 10000 using 10 processes.::

    using HIPP::MPI::INT;
    HIPP::MPI::Env env;
    auto world = env.world();
    // distribute the task
    int local_edges[2], sum = 0;
    if (world.rank() == 0) {
        int tot_num = 10;
        vector<int> edges{0};
        for (int i_proc = 0; i_proc < world.size(); ++i_proc)
            edges.push_back((i_proc + 1) * ceil(tot_num / world.size()));
        edges.back() = tot_num + 1; // modify the last element for un-uniform distribution
        for (int i_proc = 1; i_proc < world.size(); ++i_proc)
            world.send(i_proc, 0, &edges[i_proc], 2, INT);
        local_edges[0] = edges[0];
        local_edges[1] = edges[1];
    } else {
        world.recv(0, 0, &local_edges[0], 2, INT);
    }
    // do computation on each process
    for (int i = local_edges[0]; i < local_edges[1]; ++i)
        sum += i;
    // collect the result
    vector<int> res(world.size());
    world.gather(&sum, 1, INT, res.data(), 1, INT, 0);
    // print the summation
    if (world.rank() == 0) {
        int tot_sum = 0;
        for (auto v: res)
            tot_sum += v;
        cout << "The summation is " << tot_sum << ", calculated with " << world.size() << " processes.";
    }

Here we employ the rank-0 process to distribute the task and collect the
results after all the processses finished the tasks. For rank-0 process, we
should not distribute the task though MPI. Otherwise, the send will block the
program and rank-0 process cannot receive.

Although we did a extremely simple task using MPI, the pattern can be applied to
any other kinds of tasks.::

    struct TaskInfo{
        // information about a task
    }

    HIPP::MPI::Env env;
    auto world = env.world();
    TaskInfo local_task;
    if (world.rank() == 0){
        vector<TaskInfo> tasks;
        // init all the tasks
        for (int i_proc = 1; i_proc < world.size(); ++i_proc)
            world.send(i_proc, 0, &takss[i_proc], sizeof(TaskInfo), HIPP::MPI::CHAR);
        local_task = tasks[0];
    } else {
        world.recv(0, 0, &local_task, sizeof(TaskInfo) / sizeof(char), HIPP::MPI::CHAR);
    }

    // do tasks

    // collect the result

Here we use `char` datatype to send and recv the data, which can only be
applied to homegeneous computer systems. Otherwise, you should mannually create
a mpi datatype to send these data or send the struct one-by-one.
