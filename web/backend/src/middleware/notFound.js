/* 404 Middleware */
const notFound = (req, res) => {
  res.status(404).json({ success: false, error: 'Not found' });
};

/* Export middleware */
export default notFound;
