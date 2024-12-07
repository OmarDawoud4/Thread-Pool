# Thread Pool 

## Overview

This project implements a **Thread Pool System** in C that manages multiple worker threads to execute tasks concurrently. By reusing threads and leveraging a dynamic job queue, this implementation reduces the overhead of thread creation and destruction, ensuring efficient and organized task execution.

![Screenshot from 2024-12-07 16-43-54](https://github.com/user-attachments/assets/bf47cc7a-77ed-48bc-90af-060ab4ff9ddf)

## Features

- **Concurrent Task Execution**: Utilizes multiple threads to handle tasks concurrently.
- **Dynamic Job Queue**: Uses a circular queue to efficiently manage and organize tasks.
- **Thread-Safe Design**: Implements mutexes and semaphores to synchronize shared resources safely.
- **Customizable Configuration**: Configure the number of threads and queue size to fit specific workloads.



