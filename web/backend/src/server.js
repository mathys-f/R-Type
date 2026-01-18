/* Import modules */
import app from './app.js';

/* Server startup */
const PORT = 8081;
app.listen(PORT, () => {
  console.log(`Server running on port ${PORT}`);
});
