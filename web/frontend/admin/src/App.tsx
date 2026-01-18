import { BrowserRouter, Routes, Route } from 'react-router-dom'
import Navbar from './components/Navbar.tsx'
import Dashboard from './pages/Dashboard.tsx'

function App() {
  return (
    <BrowserRouter>
      <Navbar />
      <Routes>
        <Route path="/" element={<Dashboard />} />
      </Routes>
    </BrowserRouter>
  )
}

export default App
