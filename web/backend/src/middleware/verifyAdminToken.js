/* Import modules */
import jwt from 'jsonwebtoken';

/* Verify Admin Token Middleware */
const verifyAdminToken = (req, res, next) => {
  let token = req.cookies.token;

  // Also check Authorization header (Bearer token)
  if (!token && req.headers.authorization) {
    const authHeader = req.headers.authorization;
    if (authHeader.startsWith('Bearer ')) {
      token = authHeader.slice(7);
    }
  }

  if (!token) {
    return res.status(401).json({ success: false, error: 'Access Denied. No token provided.' });
  }

  try {
    const decoded = jwt.verify(token, process.env.JWT_SECRET);

    // Check if user has admin role
    if (decoded.role !== 'admin') {
      return res.status(403).json({ success: false, error: 'Insufficient permissions' });
    }

    req.user = decoded;
    next();
  } catch (err) {
    res.status(401).json({ success: false, error: 'Token expired or invalid' });
  }
};

/* Export middleware */
export { verifyAdminToken };
