#pragma once

///	<summary>
/// Template for an optional type.
///	</summary>
///	<param name="ValType">A type which has an assignment operator, even if default,
///		can be moved and will cleanly deconstruct.</param>
/// <remarks>Do not derive from this class</remarks>
template<typename ValType>
struct OptionalVal
{
	/// <summary>The value.</summary>
	ValType val;
	/// <summary><b>true</b> if the value is valid, else <b>false<.</summary>
	bool isValid;

	///	<summary>
	/// OptionalVal constructor.
	///	</summary>
	/// <remarks>val is intentionally not initialized.</remarks>
	OptionalVal() : isValid(false) {}

	///	<summary>
	/// OptionalVal constructor.
	///	</summary>
	///	<param name="val">The value to set</param>
	OptionalVal(ValType val) : val(val), isValid(true) {}

	///	<summary>
	/// OptionalVal copy constructor.
	///	</summary>
	///	<param name="other">The OptionalVal instance to copy</param>
	/// <remarks>val is intentionally not copied if isValid == false.</remarks>
	OptionalVal(const OptionalVal& other)
		: isValid(other.isValid)
	{ 
		if (isValid)
			val = other.val;
	}

	///	<summary>
	/// OptionalVal move constructor.
	///	</summary>
	///	<param name="other">The OptionalVal instance to move</param>
	/// <remarks>val is intentionally not moved if isValid == false.</remarks>
	OptionalVal(OptionalVal&& other)
		: isValid(other.isValid)
	{
		if (isValid)
			val = std::move(other.val);
	}

	///	<summary>
	/// OptionalVal destructor.
	///	</summary>
	virtual ~OptionalVal() {}

	///	<summary>
	/// OptionalVal assignment operator.
	///	</summary>
	///	<param name="other">The OptionalVal instance to copy</param>
	///	<returns>Reference to ValType.</returns>
	/// <remarks>val is intentionally not copied if isValid == false.</remarks>
	OptionalVal& operator=(const OptionalVal& other)
	{
		if (this != &other)
		{
			isValid = other.isValid;
			if (isValid)
				val = other.val;
		}
		return *this;
	}

	///	<summary>
	/// OptionalVal move assignment operator.
	///	</summary>
	///	<param name="other">The OptionalVal instance to move</param>
	/// <remarks>val is intentionally not moved if isValid == false.</remarks>
	OptionalVal& operator=(OptionalVal&& other)
	{
		if (this != &other)
		{
			isValid = other.isValid;
			if (isValid)
				val = std::move(other.val);
		}
		return *this;
	}

	///	<summary>
	/// Assigns a value and sets isValid to true.
	///	</summary>
	///	<param name="other">The value to assign.</param>
	void operator=(const ValType& val)
	{
		this->val = val;
		isValid = true;
	}

	///	<summary>
	///	Indicates whether the value is valid
	///	</summary>
	///	<returns><b>true</b> if the value is valid, else <b>false</b>.</returns>
	bool IsValid() const
	{
		return isValid;
	}

	///	<summary>
	/// Determines whether the contained value is valid and equal to a passed value.
	///	</summary>
	///	<param name="otherVal">The value to check.</param>
	///	<returns><b>true</b> if this value is valid and equal to the passed value,
	///		else <b>false</b>.</returns>
	bool IsValidAndEqual(const ValType& otherVal)
	{
		return isValid && val == otherVal;
	}

	///	<summary>
	/// Determines whether the contained value is invalid or equal to a passed value.
	///	</summary>
	///	<param name="otherVal">The value to check.</param>
	///	<returns><b>true</b> if this value is invalid or equal to the passed value,
	///		else <b>false</b>.</returns>
	bool IsInvalidOrEqual(const ValType& otherVal)
	{
		return !isValid || val == otherVal;
	}

	///	<summary>
	///	Indicates whether the value is valid
	///	</summary>
	///	<returns><b>true</b> if the value is valid, else <b>false</b>.</returns>
	operator bool() const
	{
		return isValid;
	}

	///	<summary>
	///	Sets isValid to false.
	///	</summary>
	void Reset()
	{
		isValid = false;
	}
};
