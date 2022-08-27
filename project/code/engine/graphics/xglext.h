
#if defined( TARGET_OPENGLES )
XGLEXTFN( void, glWeightPointerOES, (GLint, GLenum, GLsizei, const GLvoid *) )
XGLEXTFN( void, glMatrixIndexPointerOES, (GLint, GLenum, GLsizei, const GLvoid *) )
#else
XGLEXTFN( void, glWeightPointerARB, (GLint, GLenum, GLsizei, const GLvoid *) )
XGLEXTFN( void, glMatrixIndexPointerARB, (GLint, GLenum, GLsizei, const GLvoid *) )
#endif

#if defined( TARGET_MSWIN )
XGLEXTFN( void, glGenBuffers, (GLsizei, GLuint *) )
XGLEXTFN( void, glDeleteBuffers, (GLsizei, const GLuint *) )
XGLEXTFN( void, glBindBuffer, (GLenum, GLuint) )
XGLEXTFN( void, glBufferData, (GLenum, GLsizeiptr, const GLvoid *, GLenum) )
XGLEXTFN( void, glClientActiveTexture, (GLenum) )
XGLEXTFN( void, glActiveTexture, (GLenum) )
XGLEXTFN( void, glVertexAttribPointerARB, (GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *) )
XGLEXTFN( void, glEnableVertexAttribArrayARB, (GLuint) )
XGLEXTFN( void, glDisableVertexAttribArrayARB, (GLuint) )
XGLEXTFN( void, glCompressedTexImage2D, (GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *) )
#endif
