#if !defined(TETRIS_MATH_H)

union v2
{
    struct
    {
        int32 X, Y;
    };
    int32 E[2];
};

inline v2 operator+(v2 A, v2 B)
{
    v2 Result = {};
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;

    return Result;
}

#define TETRIS_MATH_H
#endif
