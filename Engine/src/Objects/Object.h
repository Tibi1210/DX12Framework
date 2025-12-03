#pragma once

namespace Engine{

    class Object {

    public:
        Object() = default;
        virtual ~Object() = default;
        virtual void Release();

        unsigned int id = 0;

        std::string name = "";
        std::vector<std::unique_ptr<Object>> elements;
    private:

    };
}

