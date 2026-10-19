#ifndef COREUTILS_HPP_
    #define COREUTILS_HPP_

enum class FieldType {
    Unknown,
    Float,
    Vec3,
    Quat,
    Int,
    Uint,
    Bool,
};

struct FieldDescriptor {
    const char *name;
    FieldType type;
    void *ptr;
};

#define ATOM_REFLECT(Type, ...)\
    static std::vector<FieldDescriptor> describe(Type &c) {\
        return { __VA_ARGS__ };\
    }

#define ATOM_FIELD(name, type, member)\
    FieldDescriptor(name, FieldType::type, &c.member)


#endif /* !COREUTILS_HPP_ */
