
# Quantum Bloch Sphere Visualizer

![Bloch Sphere Visualization](https://via.placeholder.com/800x400?text=Quantum+Bloch+Sphere+Visualization)

An interactive 3D visualization tool for quantum states using the Bloch sphere representation, implemented in C++ with OpenGL.

## What is This?

This project provides a real-time visualization of quantum bits (qubits) using the Bloch sphere model, which is a geometric representation of quantum states. It demonstrates fundamental quantum gates operations with immediate visual feedback.

Key Features:
- Real-time 3D visualization of qubit states
- Implementation of core quantum gates (Hadamard, Pauli-X/Y/Z)
- Interactive controls for quantum state manipulation
- Educational tool for quantum computing concepts

## How to Use It

### Prerequisites
- C++17 compatible compiler
- CMake (version 3.10 or higher)
- OpenGL (version 3.3 or higher)
- GLFW
- GLEW
- GLM

### Installation
```bash
git clone https://github.com/NyxObscura/quantum-bloch-sphere.git
cd quantum-bloch-sphere
mkdir build && cd build
cmake ..
make
```

### Running the Application
```bash
./QuantumBlochSphere
```

### Controls
- **H**: Apply Hadamard gate
- **X**: Apply Pauli-X gate
- **Y**: Apply Pauli-Y gate
- **Z**: Apply Pauli-Z gate
- **R**: Apply X-axis rotation
- **SPACE**: Toggle auto-rotation
- **ESC**: Exit application

## Development Roadmap
- [ ] Add multi-qubit visualization
- [ ] Implement quantum entanglement visualization
- [ ] Add measurement probability display
- [ ] Create tutorial mode with guided exercises

## Contributing
Contributions are welcome! Please fork the repository and create a pull request with your changes.

## Credits
**Developed by NyxObscura**  
- GitHub: [NyxObscura](https://github.com/NyxObscura)  
- Website: [obscuraworks.com](https://obscuraworks.com)  
- Email: service@obscuraworks.com  
- WhatsApp: +62 851-8334-3636  
- X (Twitter): [@ZafieroAnyiera](https://twitter.com/ZafieroAnyiera)  
- YouTube: [Obscuraworks](https://youtube.com/Obscuraworks)  
- Telegram: [@ObscuraStudent](https://t.me/ObscuraStudent)  

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

*"Visualizing the quantum world one sphere at a time"*
