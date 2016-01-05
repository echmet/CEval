#include "hvl.hpp"

namespace echmet {
        namespace math {
                #include "hvl_types.hpp"

                /* Implementation for HVL_dll::HVLData class */
                HVL_dll::HVLData::HVLData(HVLInternalValues *values, HVL_dll::frpointer freefunc, HVL_dll::alpointer allocfunc) :
                        m_values(values),
                        m_freefunc(freefunc),
                        m_allocfunc(allocfunc) {}
                HVL_dll::HVLData::HVLData(const HVLData& other)
                {
                        CopyContent(*this, other);
                }
                HVL_dll::HVLData::HVLData() : m_values(NULL) {}
                HVL_dll::HVLData::~HVLData()
                {
                        if (m_values == nullptr)
                                return;
                        m_freefunc(m_values);
                }

                size_t HVL_dll::HVLData::count() const
                {
                        if (m_values == nullptr)
                                return 0;
                        return m_values->count;
                }

                double& HVL_dll::HVLData::x(const size_t idx)
                {
                        if (m_values == NULL || m_values->count <= idx)
                                throw std::out_of_range("Index is out of range");
                        return m_values->p[idx].x;
                }
                double& HVL_dll::HVLData::y(const size_t idx)
                {
                        if (m_values == NULL || m_values->count <= idx)
                                throw std::out_of_range("Index is out of range");
                        return m_values->p[idx].y;
                }

                HVL_dll::HVLData& HVL_dll::HVLData::operator=(const HVLData& other)
                {
                        if (m_values != NULL)
                                m_freefunc(m_values);
                        CopyContent(*this, other);

                        return *this;
                }

                void HVL_dll::HVLData::CopyContent(HVL_dll::HVLData& to, const HVL_dll::HVLData& from)
                {
                        to.m_freefunc = from.m_freefunc;
                        to.m_allocfunc = from.m_allocfunc;
                        to.m_values = to.m_allocfunc(from.m_values->count);
                        if (to.m_values == NULL)
                                return;

                        memcpy(to.m_values->p, from.m_values->p, sizeof(HVLInternalPair) * from.m_values->count);
                }
        }
}

