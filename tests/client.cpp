#include "myRedis/Client.h"

int main()
{
    Application app;
    app.initConnection();
    myRedis::str::Response res {};

    app.set("first-name", "achref", res);
    if(res.status_code == 0 || res.status_code == 1){
        std::cout << "++ Command set : " <<
                  "first-name " << "achref\n" ;
    }else{
        std::cout << "Error with set command\n";
    }

    app.set("last-name", "abidi", res);
    if(res.status_code == 0 || res.status_code == 1){
        std::cout << "++ Command set : " <<
                  "last-name " << "abidi\n" ;
    }else{
        std::cout << "Error with set command\n";
    }

    app.get("last-name", res);
    if(res.status_code == 2){
        std::cout << "++ Command get : " <<res.data<< "\n" ;
    }else{
        std::cout << "Error with get command\n";
    }

    app.del("first-name", res);
    if(res.status_code == 3){
        std::cout << "++ Command del : " << res.data << "\n" ;
    }else{
        std::cout << "Error with del command\n";
    }

    std::cout << "++ Command get : last-name"<< "\n" ;
    app.get("first-name", res);
    if(res.status_code == 2){
        std::cout << "\tValue : " <<res.data<< "\n" ;
    }else{
        std::cout << "\tError with get command\n";
    }

    return 0;
}