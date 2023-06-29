# Introduction
This work is inspired by the book [Build your own Redis with C/C++][src_1].

# Commit History
1. Creating a `server()` function and a `client()` function to send and receive simple hello world.
    - Commit : [6495b66][commit_1].
    - Coincides with `Part1.03.Hello server/client` of the book [Build your own Redis with C/C++][src_1].

2. Adding protocol parsing capabilities.
    - Commit : [232f0c4][commit_2].
    - Coincides with `Part1.04.Protocol parsing` of the book [Build your own Redis with C/C++][src_1].

3. Implementing an Asyncronous server using `Event Loops`.
    - Commit: current one
    - Coincides with `Part1.05.The event loop and nonblocking IO` and  `Part1.06.The event loop implementation` of the book [Build your own Redis with C/C++][src_1].

[commit_1]: https://github.com/achref-abidi/myRedis/commit/6495b66098fbe6f67a992d25168726808ce7c246
[commit_2]: https://github.com/achref-abidi/myRedis/commit/232f0c4f565c1a589a982b8cf5166480245b200d
[src_1]: https://build-your-own.org/redis/