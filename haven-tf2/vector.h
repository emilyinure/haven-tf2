#pragma once

constexpr double pi = 3.1415926535897932384; // pi
constexpr double pi_2 = pi * 2.;               // pi * 2
__forceinline constexpr float deg_to_rad( float val ) {
	return val * ( pi / 180. );
}

// radians to degrees.
__forceinline constexpr float rad_to_deg( float val ) {
	return val * ( 180. / pi );
}


class vector {
public:
	float m_x, m_y, m_z;
	static void sin_cos( float r, float* s, float* c ) {
		*s = std::sin( r );
		*c = std::cos( r );
	}
	void init(float x, float y, float z) {
		m_x = x;
		m_y = y;
		m_z = z;
	}
	void init() {
		m_x = 0;
		m_y = 0;
		m_z = 0;
	}
	vector( ) { init( ); }
	void init(vector other) {
		init(other.m_x, other.m_y, other.m_z);
	}

	vector(float x, float y, float z) {
		init(x, y, z);
	}

	vector( const float* other ) {
		m_x = other[ 0 ];
		m_y = other[ 1 ];
		m_z = other[ 2 ];
	}

	bool const operator!=( const vector other ) const {
		int ret = 0;
		ret |= ( 1 << ( m_x != other.m_x ) );
		ret |= ( 1 << ( m_y != other.m_y ) );
		ret |= ( 1 << ( m_z != other.m_z ) );
		return ret & ( 1 << 1 );
	}

	vector const operator+(const vector other) const {
		vector ret = *this;
		ret.m_x += other.m_x;
		ret.m_y += other.m_y;
		ret.m_z += other.m_z;
		return ret;
	}
	vector const operator-(const vector other) const {
		vector ret = *this;
		ret.m_x -= other.m_x;
		ret.m_y -= other.m_y;
		ret.m_z -= other.m_z;
		return ret;
	}
	vector operator*(const vector other) const {
		vector ret = *this;
		ret.m_x *= other.m_x;
		ret.m_y *= other.m_y;
		ret.m_z *= other.m_z;
		return ret;
	}
	const vector operator/(const vector other) const {
		assert( other.m_x );
		assert( other.m_y );
		assert( other.m_z );
		vector ret = *this;
		ret.m_x /= other.m_x;
		ret.m_y /= other.m_y;
		ret.m_z /= other.m_z;
		return ret;
	}
	vector operator/=(const vector other) {
		assert( other.m_x );
		assert( other.m_y );
		assert( other.m_z );
		vector ret = *this;
		m_x /= other.m_x;
		m_y /= other.m_y;
		m_z /= other.m_z;
		return *this;
	}
	vector operator*=(const vector other) {
		vector ret = *this;
		m_x *= other.m_x;
		m_y *= other.m_y;
		m_z *= other.m_z;
		return *this;
	}
	vector operator-=(const vector other) {
		vector ret = *this;
		m_x -= other.m_x;
		m_y -= other.m_y;
		m_z -= other.m_z;
		return *this;
	}


	vector operator+=(const vector other) {
		vector ret = *this;
		m_x += other.m_x;
		m_y += other.m_y;
		m_z += other.m_z;
		return *this;
	}
	vector operator+(const float other) const {
		vector ret = *this;
		ret.m_x += other;
		ret.m_y += other;
		ret.m_z += other;
		return ret;
	}
	vector operator-(const float other) const {
		vector ret = *this;
		ret.m_x -= other;
		ret.m_y -= other;
		ret.m_z -= other;
		return ret;
	}
	vector operator*(const float other) const {
		vector ret = *this;
		ret.m_x *= other;
		ret.m_y *= other;
		ret.m_z *= other;
		return ret;
	}
	vector operator/(const float other) const {
		assert( other );
		vector ret = *this;
		ret.m_x /= other;
		ret.m_y /= other;
		ret.m_z /= other;
		return ret;
	}
	vector operator/=(const float other) {
		assert( other );
		vector ret = *this;
		m_x /= other;
		m_y /= other;
		m_z /= other;
		return *this;
	}
	vector operator*=(const float other) {
		vector ret = *this;
		m_x *= other;
		m_y *= other;
		m_z *= other;
		return *this;
	}
	vector operator-=(const float other) {
		vector ret = *this;
		m_x -= other;
		m_y -= other;
		m_z -= other;
		return *this;
	}
	vector operator+=( const float other ) {
		vector ret = *this;
		m_x += other;
		m_y += other;
		m_z += other;
		return *this;
	}
	bool operator<=( const float other ) const {
		return fabsf( m_x ) <= other &&
			fabsf( m_y ) <= other &&
			fabsf( m_z ) <= other;
	}
	bool operator<=( const vector other ) const {
		return fabsf( m_x ) <= other.m_x &&
			fabsf( m_y ) <= other.m_y &&
			fabsf( m_z ) <= other.m_z;
	}
	bool operator>=( const vector other ) const {
		return fabsf( m_x ) >= other.m_x ||
			fabsf( m_y ) >= other.m_y ||
			fabsf( m_z ) >= other.m_z;
	}
	bool operator==( const vector &other ) const {
		return ( m_x ) == other.m_x &&
			( m_y ) == other.m_y &&
			( m_z ) == other.m_z;
	}
	bool operator>=( const float other ) const {
		return fabsf( m_x ) >= other ||
			fabsf( m_y ) >= other ||
			fabsf( m_z ) >= other;
	}
	bool operator<( const float other ) const {
		return fabsf( m_x ) < other &&
			fabsf( m_y ) < other &&
			fabsf( m_z ) < other;
	}
	float &operator[](const int i) {
		if ( i == 0 )
			return m_x;
		if ( i == 1 )
			return m_y;
		if ( i == 2 )
			return m_z;
	}

	float length() const {
		const float ln_sqr = length_sqr( );
		if ( ln_sqr == 0.f )
			return 0;
		const float ln = sqrtf( ln_sqr );
		if (isnan(ln) || ln == 0.f)
			return 0;
		return ln;
	}
	float length_2d() const {
		const float ln_sqr = length_sqr_2d();
		if ( ln_sqr == 0.f )
			return 0;
		const float ln = sqrtf( ln_sqr );
		if (isnan(ln) || ln == 0.f)
			return 0;
		return ln;
	}
	float length_sqr( ) const {
		const float ln = ( m_x * m_x ) + ( m_y * m_y ) + ( m_z * m_z );
		if ( isnan( ln ) || ln == 0.f )
			return 0;
		return ln;
	}
	float length_sqr_2d( ) const {
		const float ln = ( m_x * m_x ) + ( m_y * m_y );
		if ( isnan( ln ) || ln == 0.f )
			return 0;
		return ln;
	}
	__forceinline float dot(const vector& b) const {
		return (this->m_x * b.m_x + this->m_y * b.m_y + this->m_z * b.m_z);
	}
	float normalize_in_place( ) {
		const float ln = length( );
		if ( isnan( ln ) || ln == 0.f )
			return 0;
		m_x /= ln;
		m_y /= ln;
		m_z /= ln;
		return ln;
	};
	vector normalized( ) const {
		const float ln = length( );
		if ( isnan( ln ) || ln == 0.f )
			return vector();
		vector ret = *this;
		ret.m_x /= ln;
		ret.m_y /= ln;
		ret.m_z /= ln;
		return ret;
	};
	inline vector vector_ma(float scale, const vector& direction)
	{
		vector dest;
		dest.m_x = this->m_x + scale * direction.m_x;
		dest.m_y = this->m_y + scale * direction.m_y;
		dest.m_z = this->m_z + scale * direction.m_z;
		return dest;
	}
	inline void cross_product(const vector& b, vector& result)
	{
		result.m_x = this->m_y * b.m_z - this->m_z * b.m_y;
		result.m_y = this->m_z * b.m_x - this->m_x * b.m_z;
		result.m_z = this->m_x * b.m_y - this->m_y * b.m_x;
	}
	vector look( vector target ) const {
		target -= *this;
		vector angles;
		if ( target.m_y == 0.0f && target.m_x == 0.0f ) {
			angles.m_x = ( target.m_z > 0.0f ) ? 270.0f : 90.0f;
			angles.m_y = 0.0f;
		}
		else {
			angles.m_x = static_cast< float >( atan2( -target.m_z, target.length_2d( ) ) * 180.f / pi );
			angles.m_y = static_cast< float >( atan2( target.m_y, target.m_x ) * 180.f / pi );

			//if ( angles.y > 90 )
			//	angles.y -= 180;
			//else if ( angles.y < 90 )
			//	angles.y += 180;
			//else if ( angles.y == 90 )
			//	angles.y = 0;
		}

		angles.m_z = 0.0f;
		return angles;
	}
	vector angle_to( ) const {
		vector angles;
		if ( m_y == 0.0f && m_x == 0.0f ) {
			angles.m_x = ( m_z > 0.0f ) ? 270.0f : 90.0f;
			angles.m_y = 0.0f;
		}
		else {
			angles.m_x = static_cast< float >( atan2( -m_z, length_2d( ) ) * 180.f / pi );
			angles.m_y = static_cast< float >( atan2( m_y, m_x ) * 180.f / pi );

			//if ( angles.y > 90 )
			//	angles.y -= 180;
			//else if ( angles.y < 90 )
			//	angles.y += 180;
			//else if ( angles.y == 90 )
			//	angles.y = 0;
		}

		angles.m_z = 0.0f;
		return angles;
	}

	vector angle_vector( ) const {
		vector forward;
		float sp, sy, sr, cp, cy, cr;

		sin_cos( deg_to_rad( this->m_x ), &sp, &cp );
		sin_cos( deg_to_rad( this->m_y ), &sy, &cy );

		forward.m_x = cp * cy;
		forward.m_y = cp * sy;
		forward.m_z = -sp;
		return forward;
	}

	[[nodiscard]] float delta( const vector view ) const {
		const auto dir = this->normalized();

		// get the forward direction vector of the view angles.
		const auto fw = view.angle_vector( );

		// get the angle between the view angles forward directional vector and the target location.
		const float result = rad_to_deg( std::acosf( fw.dot( dir ) ));
		return result;
	}

	void transform( const matrix_3x4& matrix, vector& out ) {
		out.m_x = dot( matrix[ 0 ] ) + matrix[ 0 ][ 3 ];
		out.m_y = dot( matrix[ 1 ] ) + matrix[ 1 ][ 3 ];
		out.m_z = dot( matrix[ 2 ] ) + matrix[ 2 ][ 3 ];
	}


	void angle_vectors( vector* forward, vector* right, vector* up ) {
		float sp, sy, sr, cp, cy, cr;

		vector::sin_cos( deg_to_rad( this->m_x ), &sp, &cp );
		vector::sin_cos( deg_to_rad( this->m_y ), &sy, &cy );
		vector::sin_cos( deg_to_rad( this->m_z ), &sr, &cr );

		if ( forward ) {
			forward->m_x = cp * cy;
			forward->m_y = cp * sy;
			forward->m_z = -sp;
		}

		if ( right ) {
			right->m_x = -1 * sr * sp * cy + -1 * cr * -sy;
			right->m_y = -1 * sr * sp * sy + -1 * cr * cy;
			right->m_z = -1 * sr * cp;
		}

		if ( up ) {
			up->m_x = cr * sp * cy + -sr * -sy;
			up->m_y = cr * sp * sy + -sr * cy;
			up->m_z = cr * cp;
		}
	}
};

class __declspec( align( 16 ) )vector_aligned : public vector {
public:
	inline vector_aligned( void ) {};

	inline vector_aligned( float X, float Y, float Z ) {
		init( X, Y, Z );
	}

	explicit vector_aligned( const vector& vOther ) {
		init( vOther.m_x, vOther.m_y, vOther.m_z );
	}

	vector_aligned& operator=( const vector& vOther ) {
		init( vOther.m_x, vOther.m_y, vOther.m_z );
		return *this;
	}

	float w;
};