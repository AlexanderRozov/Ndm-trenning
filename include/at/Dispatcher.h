#include "at/command.h"

#include <functional>
#include <string>
#include <unordered_map>

namespace at
{

    class Dispatcher
    {
    public:
        using Handler = std::function<std::string(const Command&)>;

        void registerCommand(
            std::string name,
            Handler handler
        );

        std::string dispatch(const Command& command) const;

    private:
        std::unordered_map<std::string, Handler> handlers_;
    };

}