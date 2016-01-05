#ifndef ECHMET_MATH_HVL_TYPES_HPP
#define ECHMET_MATH_HVL_TYPES_HPP

struct HVLInternalPair {
	double x;
	double y;
};

struct HVLInternalValues {
	HVLInternalPair *p;	/*!< Calculated values of HVL function */
	size_t count;		/*!< Number of results */
};

#endif //ECHMET_MATH_HVL_TYPES_HPP
