#pragma once

// These intentionally inline to keep them as light weight as possible
// These really aren't manipulators, but close enough to use the name.

struct Manipulators
{
	///	<summary>
	/// An enumeration of manipulator types.
	///	</summary>
	enum Type
	{
		Log,
		Html
	};

	///	<summary>
	/// Returns the type of manipulator being used.
	///	</summary>
	///	<returns>The type of manipulator being used.</returns>
	virtual Type GetType() const = 0;

	///	<summary>
	/// Returns an end-of-line string appropriate to the manipulator.
	///	</summary>
	///	<returns>An end-of-line string appropriate to the manipulator.</returns>
	virtual const char* endl() const
	{
		return "";
	}

	///	<summary>
	/// Returns a start indent string appropriate to the manipulator.
	///	</summary>
	///	<returns>A start indent string appropriate to the manipulator.</returns>
	virtual const char* startindent()
	{
		return "";
	}

	///	<summary>
	/// Appends an indent appropriate to the manipulator.
	///	</summary>
	///	<param name="ostr">A reference to a std::ostream.</param>
	///	<returns>A reference to the passed ostream.</returns>
	virtual std::ostream& indent(std::ostream& ostr) const
	{
		return ostr;
	}

	///	<summary>
	/// Returns an end indent string appropriate to the manipulator.
	///	</summary>
	///	<returns>A start indent string appropriate to the manipulator.</returns>
	virtual const char* endindent()
	{
		return "";
	}

	///	<summary>
	///	This class provides a scope for a indentation.
	///	</summary>
	class Indent
	{
	private:
		std::ostream& m_ostr;
		Manipulators& m_manipulators;

	public:
		///	<summary>
		/// Indent constructor.
		///	</summary>
		///	<param name="ostr">A reference to a std::ostream.</param>
		///	<param name="ostr">A reference to a Manipulator.</param>
		Indent(std::ostream& ostr, Manipulators& manipulators)
			: m_ostr(ostr)
			, m_manipulators(manipulators)
		{
			m_ostr << m_manipulators.startindent();
		}

		///	<summary>
		/// Indent destructor.
		///	</summary>
		~Indent()
		{
			m_ostr << m_manipulators.endindent();
		}
	};
};

struct LogManipulators : public Manipulators
{
	///	<summary>
	/// LogManipulators constructor.
	///	</summary>
	LogManipulators()
		: numIndents(0)
	{
	}

	///	<summary>
	/// Returns Log type.
	///	</summary>
	///	<returns>Log type.</returns>
	virtual Type GetType() const
	{
		return Log;
	}

	///	<summary>
	/// Returns an carriage return.
	///	</summary>
	///	<returns>A carriage return.</returns>
	virtual const char* endl() const
	{
		return "\n";
	}

	///	<summary>
	/// Increments the indentation count and returns a zero length string.
	///	</summary>
	///	<returns>An empty string.</returns>
	virtual const char* startindent()
	{
		numIndents++;
		return "";
	}

	///	<summary>
	/// Appends spaces equal to the current indentation.
	///	</summary>
	///	<param name="ostr">A reference to a std::ostream.</param>
	///	<returns>A reference to the passed ostream.</returns>
	virtual std::ostream& indent(std::ostream& ostr) const
	{
		for (unsigned i = 0; i < numIndents; i++)
		{
			ostr << "    ";
		}
		return ostr;
	}

	///	<summary>
	/// Decrements the indentation count and returns a zero length string.
	///	</summary>
	///	<returns>An empty string.</returns>
	virtual const char* endindent()
	{
		numIndents--;
		return "";
	}

private:
	unsigned numIndents;
};

struct HtmlManipulators : public Manipulators
{
	///	<summary>
	/// Returns Html type.
	///	</summary>
	///	<returns>Html type.</returns>
	virtual Type GetType() const
	{
		return Html;
	}

	///	<summary>
	/// Returns an HTML break.
	///	</summary>
	///	<returns>An HTML break.</returns>
	virtual const char* endl() const
	{
		return "<br>";
	}

	///	<summary>
	/// Returns the start of an div indent style.
	///	</summary>
	///	<returns>The start of an indent style.</returns>
	virtual const char* startindent()
	{
		return "<div style=\"margin-left:6px;\">";
	}

	///	<summary>
	/// Returns the a div close.
	///	</summary>
	///	<returns>A div close.</returns>
	virtual const char* endindent()
	{
		return "</div>";
	}
};

