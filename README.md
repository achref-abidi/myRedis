# Introduction
This work is inspired by the book [Build your own Redis with C/C++][src_1].

# Commit History
2. Creating a `server()` function and a `client()` function to send and receive simple hello world.
    - Commit : [6495b66][commit_2].
    - Coincides with `Part1.03.Hello server/client` of the book [Build your own Redis with C/C++][src_1].

3. Adding protocol parsing capabilities.
    - Commit : [232f0c4][commit_3].
    - Coincides with `Part1.04.Protocol parsing` of the book [Build your own Redis with C/C++][src_1].

4. Implementing an Asyncronous server using `Event Loops`.
    - Commit : [ba542d4][commit_4].
    - Coincides with `Part1.05.The event loop and nonblocking IO` and  `Part1.06.The event loop implementation` of the book [Build your own Redis with C/C++][src_1].

5. Some clean up
    - Commit : [62fa1e0][commit_5].
        - Renaming `Connection.h` to `Client.h`.
        - Renaming `Connection.cpp` to `Client.cpp`
        - Making a class for Conn (Connection.h).
        - Making a class for AsyncServer.
        - ...
6. Adding basic commands (get, del, set) to the server
   - Commit : current one.
   - Coincides with `Part1.07.Basic Server: get, set, del` of the book [Build your own Redis with C/C++][src_1].

[commit_2]: https://github.com/achref-abidi/myRedis/commit/6495b66098fbe6f67a992d25168726808ce7c246
[commit_3]: https://github.com/achref-abidi/myRedis/commit/232f0c4f565c1a589a982b8cf5166480245b200d
[commit_4]: https://github.com/achref-abidi/myRedis/commit/ba542d4fcbacea59f74a181d5ef2f06751c5049e
[commit_5]: https://github.com/achref-abidi/myRedis/commit/62fa1e06466b210ab15feca249835bd6fac4e895
[src_1]: https://build-your-own.org/redis/