#pragma once

namespace Engine{

    class Object {

    public:
        Object() = default;
        virtual ~Object() = default;
        virtual void Release();

        std::vector<std::unique_ptr<Object>> elements;
    private:

    };
}

