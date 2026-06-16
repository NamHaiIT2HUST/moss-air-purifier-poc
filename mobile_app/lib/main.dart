import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';

// Tạm thời ẩn Firebase khởi tạo để test UI tĩnh trước cho chắc ăn
void main() {
  WidgetsFlutterBinding.ensureInitialized();
  runApp(const MossApp());
}

class MossApp extends StatelessWidget {
  const MossApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Moss Air Purifier',
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        brightness: Brightness.dark,
        scaffoldBackgroundColor: const Color(0xFF121212),
        textTheme: GoogleFonts.montserratTextTheme(ThemeData.dark().textTheme),
      ),
      home: const DashboardScreen(),
    );
  }
}

class DashboardScreen extends StatefulWidget {
  const DashboardScreen({super.key});

  @override
  State<DashboardScreen> createState() => _DashboardScreenState();
}

class _DashboardScreenState extends State<DashboardScreen> {
  // Dữ liệu tĩnh (Mockup Data) để test giao diện và logic cảnh báo
  Map<String, dynamic> sensorData = {
    "temperature": 28.5,
    "humidity": 55.0, // Cố tình để dưới 60 để test cảnh báo đỏ
    "pm25": 45.0,
    "fan_status": "ON",
    "humidifier_status": "ON",
    "moss_status": "Healthy"
  };

  @override
  Widget build(BuildContext context) {
    // Logic: Nếu độ ẩm < 60 thì biến isDry = true
    bool isDry = sensorData['humidity'] < 60.0;
    bool isDusty = sensorData['pm25'] > 25.0;

    return Scaffold(
      appBar: AppBar(
        title: const Text('MOSS ECOSYSTEM', style: TextStyle(fontWeight: FontWeight.bold, color: Colors.white)),
        backgroundColor: const Color(0xFF1E1E1E),
        elevation: 0,
        centerTitle: true,
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text('Live Environment', style: TextStyle(fontSize: 20, color: Colors.grey[400])),
            const SizedBox(height: 20),
            Expanded(
              child: GridView.count(
                crossAxisCount: 2,
                crossAxisSpacing: 16,
                mainAxisSpacing: 16,
                children: [
                  _buildCard('PM 2.5', '${sensorData['pm25']}', 'µg/m³', Icons.air, isDusty, null),
                  _buildCard('Humidity', '${sensorData['humidity']}', '%', Icons.water_drop, isDry, null),
                  _buildCard('Temperature', '${sensorData['temperature']}', '°C', Icons.thermostat, false, null),
                  _buildCard('Moss Health', sensorData['moss_status'], '', Icons.eco, false, Colors.green),
                ],
              ),
            ),
            // Thanh cảnh báo phía dưới màn hình
            Container(
              padding: const EdgeInsets.all(16),
              decoration: BoxDecoration(
                color: isDry ? Colors.redAccent.withOpacity(0.2) : const Color(0xFF1E1E1E),
                borderRadius: BorderRadius.circular(12),
                border: Border.all(color: isDry ? Colors.redAccent : Colors.transparent),
              ),
              child: Row(
                children: [
                  Icon(isDry ? Icons.warning_amber_rounded : Icons.check_circle, 
                       color: isDry ? Colors.redAccent : Colors.green),
                  const SizedBox(width: 10),
                  Expanded(
                    child: Text(
                      isDry ? 'SYSTEM ALERT: Activating Humidifier...' : 'System running optimally',
                      style: TextStyle(color: isDry ? Colors.redAccent : Colors.green, fontWeight: FontWeight.bold),
                    ),
                  ),
                ],
              ),
            )
          ],
        ),
      ),
    );
  }

  Widget _buildCard(String title, String value, String unit, IconData icon, bool isWarning, Color? overrideColor) {
    Color mainColor = overrideColor ?? (isWarning ? Colors.redAccent : Colors.blueAccent);
    return Container(
      padding: const EdgeInsets.all(16),
      decoration: BoxDecoration(
        color: const Color(0xFF1E1E1E),
        borderRadius: BorderRadius.circular(16),
        border: Border.all(color: mainColor.withOpacity(0.5), width: 2),
      ),
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          Icon(icon, size: 40, color: mainColor),
          const SizedBox(height: 10),
          Text(title, style: const TextStyle(color: Colors.grey, fontSize: 14)),
          const SizedBox(height: 5),
          Row(
            mainAxisAlignment: MainAxisAlignment.center,
            crossAxisAlignment: CrossAxisAlignment.baseline,
            textBaseline: TextBaseline.alphabetic,
            children: [
              Text(value, style: const TextStyle(color: Colors.white, fontSize: 28, fontWeight: FontWeight.bold)),
              Text(' $unit', style: const TextStyle(color: Colors.grey, fontSize: 14)),
            ],
          ),
        ],
      ),
    );
  }
}