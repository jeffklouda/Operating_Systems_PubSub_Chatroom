CSE.30341.FA17: Project 03
==========================

This is the documentation for [Project 03] of [CSE.30341.FA17].

Members
-------

1. Matthew D'Alonzo (mdalonz1@nd.edu) 
2. Jeff Klouda (jklouda1@nd.edu)

Design
------

> 1. The client library needs to provide a `Client` class.
>
>   - What does the `Client` class need to keep track of?
>       The `Client` class needs to keep track of the `host`, the server `port`, the client identity `cid`, and the topics it is subscribed to.
>   - How will the `Client` connect to the server?
>       The `Client` will connect to the server via streaming network sockets. We will implement this using the socket() and connect() system calls.
>   - How will it implement concurrent publishing, retrieval, and processing of
>     messages?
>       We will implement this using POSIX threads.
>   - How will it pass messages between different threads?
>
>       The class will pass messages between different threads using concurrent data structures such as the queue we created in Lecture 11: Condition Variables.
>
>   - What data members need to have access synchronized? What primitives will
>     you use to implement these?
>       
>       The topics the client is subscribed to and the socket file descriptor need to have access synchronized. We will use semaphores to to implement this.
>
>   - How will threads know when to block or to quit?
>
>       Threads will know when to block based on semaphore values, and they will quit when they finish their execution.
>
>   - How will the `Client` determine which `Callbacks` belong to which topics?
>
>       Whenever a Callback is created, it will have an associated topic, so the client always knows which Callbacks belong to which topics.      
>

> 2. The client library needs to provide a `Callback` class.
>
>   - What does the `Callback` class need to keep track of?
>
>       
>
>   - How will applications use this `Callback` class?

> 3. The client library needs to provide a `Message` struct.
>
>   - What does the `Message` struct need to keep track of?
>
>   - What methods would be useful to have in processing `Messages`?

>       The `Message` struct needs to keep track of the Message's type, topic, sender, and body as well as the sender's nonce. Some useful methods in processing messages would be an echoCallback that echoes the message received, a queueCallback that stores messages received in a queue, and a initializeCallback to create new messages.

> 4. The client library needs to provide a `Thread` class.
>
>   - What does the `Thread` class need to keep track of?
>
>       The `Thread` class needs to keep track of the thread_func and its arguments.
>
>   - What POSIX thread functions will need to utilize?
>
>       The program will utilize pthread_create(), pthread_join(), pthread_detatch() and pthread_exit().

> 5. You will need to perform testing on your client library.
>
>   - How will you test your client library?
>
>   - What will `echo_test` tell you?
>
>   - How will you use the Google Test framework?
>
>   - How will you incorporate testing at every stage of development?

Response.

> 6. You will need to create an user application that takes advantage of the
>    pub/sub system.
>
>   - What distributed and parallel application do you plan on building?
>
>   - How will utilize the client library?
>
>   - What topics will you need?
>
>   - What callbacks will you need?
>
>   - What additional threads will you need?

Response.

Demonstration
-------------

> Place a link to your demonstration slides on [Google Drive].

Errata
------

> Describe any known errors, bugs, or deviations from the requirements.

Extra Credit
------------

> Describe what extra credit (if any) that you implemented.

[Project 03]:       https://www3.nd.edu/~pbui/teaching/cse.30341.fa17/project03.html
[CSE.30341.FA17]:   https://www3.nd.edu/~pbui/teaching/cse.30341.fa17/
[Google Drive]:     https://drive.google.com
