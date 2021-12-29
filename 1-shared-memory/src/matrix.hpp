#ifndef MATRIX_HPP
#define MATRIX_HPP
#include <memory>
#include <iostream>
#include <vector>
#include <omp.h>

using Index = long long;

template <typename T>
class Mat
{
    std::unique_ptr<T[]> data_;
    Index rows_, cols_;

public:
    static bool PAR_MUL;

    Mat() = default;
    Mat(Mat const &);
    Mat(Mat &&);
    Mat(Index size);
    Mat(Index rows, Index cols);
    Mat(T *data, Index rows, Index cols);

    static Mat Random(T max, Index size);
    static Mat Random(T max, Index rows, Index cols);

    Index rows() const;
    Index cols() const;
    Index size() const;

    T &operator()(Index row, Index col);
    T const &operator()(Index row, Index col) const;

    T &operator()(Index idx);
    T const &operator()(Index idx) const;

    Mat &operator=(Mat const &);
    Mat &operator=(Mat &&);

    T *data();
    T const *data() const;

    friend std::ostream &operator<<(std::ostream &out, const Mat &mat)
    {
        auto j{0};
        for (auto i{0}; i < mat.rows(); ++i)
        {
            for (j = 0; j < mat.cols() - 1; ++j)
            {
                out << mat(i, j) << '\t';
            }
            out << mat(i, j) << '\n';
        }
        return out;
    }

    Mat operator*(Mat const &oth);
    bool operator==(Mat const &oth);

    void reset();
    void transpose();

    Mat<T> transposed();
};

template <typename T>
bool Mat<T>::PAR_MUL = false;

template <typename T>
Mat<T>::Mat(Mat const &oth)
    : Mat(oth.rows_, oth.cols_)
{
    for (auto i = 0; i < oth.size(); ++i)
        data_[i] = oth(i);
}

template <typename T>
Mat<T>::Mat(Mat &&oth)
    : rows_(oth.rows_), cols_(oth.cols_), data_(std::move(oth.data_))
{
    oth.rows_ = 0, oth.cols_ = 0;
}

template <typename T>
Mat<T>::Mat(Index size)
    : rows_(size), cols_(1), data_(std::make_unique<T[]>(size))
{
}

template <typename T>
Mat<T>::Mat(Index rows, Index cols)
    : rows_(rows), cols_(cols), data_(std::make_unique<T[]>(rows * cols))
{
}

template <typename T>
Mat<T>::Mat(T *data, Index rows, Index cols)
    : rows_(rows), cols_(cols), data(std::make_unique(data))
{
}

template <typename T>
Mat<T> Mat<T>::Random(T max, Index size)
{
    auto mat = Mat(size);
    for (auto i = 0; i < mat.size(); ++i)
    {
        mat(i) = T(double(rand()) / RAND_MAX * max);
    }
    return mat;
}

template <typename T>
Mat<T> Mat<T>::Random(T max, Index rows, Index cols)
{
    auto mat = Mat(rows, cols);
    for (auto i = 0; i < mat.size(); ++i)
    {
        mat(i) = T(double(rand()) / RAND_MAX * max);
    }
    return mat;
}

template <typename T>
Index Mat<T>::rows() const
{
    return rows_;
}

template <typename T>
Index Mat<T>::cols() const
{
    return cols_;
}

template <typename T>
Index Mat<T>::size() const
{
    return rows_ * cols_;
}

template <typename T>
T &Mat<T>::operator()(Index row, Index col)
{
    return data_[row * cols_ + col];
}

template <typename T>
T const &Mat<T>::operator()(Index row, Index col) const
{
    return data_[row * cols_ + col];
}

template <typename T>
T &Mat<T>::operator()(Index idx)
{
    return data_[idx];
}

template <typename T>
T const &Mat<T>::operator()(Index idx) const
{
    return data_[idx];
}

template <typename T>
Mat<T> &Mat<T>::operator=(Mat const &oth)
{
    if (size() != oth.size())
        data_ = std::make_unique<T[]>(oth.size());

    rows_ = oth.rows_;
    cols_ = oth.cols_;

    for (auto i = 0; i < size(); ++i)
        operator()(i) = oth(i);

    return *this;
}

template <typename T>
Mat<T> &Mat<T>::operator=(Mat &&oth)
{
    rows_ = oth.rows_, oth.rows_ = 0;
    cols_ = oth.cols_, oth.cols_ = 0;

    data_ = std::move(oth.data_);

    return *this;
}

template <typename T>
T *Mat<T>::data()
{
    return data_.get();
}

template <typename T>
T const *Mat<T>::data() const
{
    return data_.get();
}

template <typename T>
Mat<T> Mat<T>::operator*(Mat const &oth)
{
    _ASSERT(cols_ == oth.rows_);

    auto res = Mat(rows_, oth.cols_);
    res.reset();

    if (PAR_MUL)
    {
        auto ri = 0, ci = 0, el = 0;

        auto &a = *this;

#pragma omp parallel shared(a, oth, res) private(ri, ci, el)
        {
#pragma omp for
            for (ri = 0; ri < rows_; ++ri)
            {
                for (ci = 0; ci < oth.cols_; ++ci)
                    for (el = 0; el < cols_; ++el)
                        res(ri, ci) += a(ri, el) * oth(el, ci);
            }
        }
    }
    else
    {

        for (auto ri = 0; ri < rows_; ++ri)
            for (auto ci = 0; ci < oth.cols_; ++ci)
                for (auto el = 0; el < cols_; ++el)
                    res(ri, ci) += operator()(ri, el) * oth(el, ci);
    }

    return res;
}

template<typename T>
bool Mat<T>::operator==(Mat const &oth)
{
    if (rows_ != oth.rows_ || cols_ != oth.cols_)
        return false;

    for(auto i = 0; i < size(); ++i)
        if (data_[i] != oth(i))
            return false; 

    return true;
}

template <typename T>
void Mat<T>::reset()
{
    for (auto i = 0; i < size(); ++i)
        data_[i] = T{};
}

template <typename T>
void Mat<T>::transpose()
{
    const auto last = size() - 1;

    auto visited = std::vector<bool>(size());
    auto cycle = Index(0);
    while (++cycle != last)
    {
        if (visited[cycle])
            continue;
        auto a = cycle;
        do
        {
            a = (a == last)
                    ? last
                    : (rows_ * a) % last;
            std::swap(operator()(a), operator()(cycle));
            visited[a] = true;
        } while (a != cycle);
    }

    std::swap(rows_, cols_);
}

template <typename T>
Mat<T> Mat<T>::transposed()
{
    auto mat = Mat(cols_, rows_);
    for (auto ri = 0; ri < rows_; ++ri)
        for (auto ci = 0; ci < cols_; ++ci)
            mat(ci, ri) = operator()(ri, ci);
    return mat;
}

using MatXd = Mat<double>;
using MatXi = Mat<int>;

#endif