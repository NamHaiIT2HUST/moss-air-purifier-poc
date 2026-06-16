import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:firebase_database/firebase_database.dart';
import 'firebase_options.dart'; // Import file cấu hình bạn vừa tạo

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  // Khởi tạo Firebase chính thức
  await Firebase.initializeApp(
    options: DefaultFirebaseOptions.currentPlatform,
  );
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
        scaffoldBackgroundColor: const Color(0xFF0D0F12),
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
  // Trỏ thẳng đến nhánh dữ liệu hiện tại trên Firebase
  final DatabaseReference _dbRef = FirebaseDatabase.instance.ref('sensors/current');
  
  Map<String, dynamic> sensorData = {
    "temperature": 0.0,
    "humidity": 0.0, 
    "pm25": 0.0,
    "fan_status": "OFF",
    "humidifier_status": "OFF",
    "moss_status": "Loading..."
  };

  bool _hasShownPopup = false;

  @override
  void initState() {
    super.initState();
    // THE MAGIC: Lắng nghe luồng dữ liệu thời gian thực
    _dbRef.onValue.listen((DatabaseEvent event) {
      final data = event.snapshot.value as Map<dynamic, dynamic>?;

      if (data != null) {
        setState(() {
          sensorData['temperature'] = (data['temperature'] ?? 0).toDouble();
          sensorData['humidity'] = (data['humidity'] ?? 0).toDouble();
          sensorData['pm25'] = (data['pm25'] ?? 0).toDouble();
          sensorData['fan_status'] = data['fan_status'] ?? 'OFF';
          sensorData['humidifier_status'] = data['humidifier_status'] ?? 'OFF';
          sensorData['moss_status'] = data['moss_status'] ?? 'Unknown';
        });
        
        _checkAlertLogic();
      }
    });
  }

  // Logic xử lý cảnh báo kết hợp Thông số Sinh học
  void _checkAlertLogic() {
    bool isDry = sensorData['humidity'] > 0 && (sensorData['humidity'] < 60.0 || sensorData['moss_status'] == 'Dehydrated');
    
    if (isDry && !_hasShownPopup) {
      _hasShownPopup = true;
      // Dùng Future.delayed để tránh lỗi hiện Dialog khi màn hình đang build
      Future.delayed(Duration.zero, () {
        _showHumidifierPopup();
      });
    }
  }

  Color _getPM25Color(double pm25) {
    if (pm25 == 0) return Colors.grey;
    if (pm25 > 55.0) return Colors.redAccent; 
    if (pm25 >= 35.0) return Colors.orangeAccent; 
    return Colors.cyanAccent; 
  }

  void _showHumidifierPopup() {
    showDialog(
      context: context,
      barrierDismissible: false,
      builder: (BuildContext context) {
        return Dialog(
          backgroundColor: Colors.transparent,
          child: Container(
            padding: const EdgeInsets.all(24),
            decoration: BoxDecoration(
              gradient: const LinearGradient(
                colors: [Color(0xFF2B1216), Color(0xFF171A21)],
                begin: Alignment.topLeft,
                end: Alignment.bottomRight,
              ),
              borderRadius: BorderRadius.circular(24),
              border: Border.all(color: Colors.redAccent, width: 2),
              boxShadow: [
                BoxShadow(color: Colors.redAccent.withOpacity(0.5), blurRadius: 30, spreadRadius: 5)
              ],
            ),
            child: Column(
              mainAxisSize: MainAxisSize.min,
              children: [
                const Icon(Icons.warning_amber_rounded, color: Colors.redAccent, size: 64),
                const SizedBox(height: 16),
                const Text('CRITICAL BIOLOGY ALERT', style: TextStyle(color: Colors.redAccent, fontSize: 20, fontWeight: FontWeight.w900)),
                const SizedBox(height: 12),
                const Text(
                  'Hypnum Moss hydration level is critically low (<60%). Auto-activation required.',
                  textAlign: TextAlign.center,
                  style: TextStyle(color: Colors.white70, fontSize: 15),
                ),
                const SizedBox(height: 24),
                ElevatedButton.icon(
                  onPressed: () {
                    Navigator.of(context).pop();
                    // Tạm thời ẩn cờ để không hiện liên tục, chờ data Firebase mới đè lên
                    _hasShownPopup = false; 
                  },
                  icon: const Icon(Icons.water_drop, color: Colors.white),
                  label: const Text('ACKNOWLEDGE', style: TextStyle(fontWeight: FontWeight.bold, color: Colors.white)),
                  style: ElevatedButton.styleFrom(
                    backgroundColor: Colors.redAccent,
                    padding: const EdgeInsets.symmetric(horizontal: 20, vertical: 14),
                    shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(12)),
                  ),
                )
              ],
            ),
          ),
        );
      },
    );
  }

  @override
  Widget build(BuildContext context) {
    bool isDry = sensorData['humidity'] > 0 && (sensorData['humidity'] < 60.0 || sensorData['moss_status'] == 'Dehydrated');
    double pm25Value = sensorData['pm25'];
    Color pm25CardColor = _getPM25Color(pm25Value);

    return Scaffold(
      body: Container(
        decoration: const BoxDecoration(
          gradient: RadialGradient(
            colors: [Color(0xFF1A1F2B), Color(0xFF0D0F12)],
            center: Alignment.topLeft,
            radius: 1.5,
          ),
        ),
        child: SafeArea(
          child: Padding(
            padding: const EdgeInsets.symmetric(horizontal: 20.0, vertical: 16.0),
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Row(
                  mainAxisAlignment: MainAxisAlignment.spaceBetween,
                  children: [
                    Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        Text('MOSS ECOSYSTEM', style: TextStyle(fontSize: 24, fontWeight: FontWeight.w900, letterSpacing: 1.2, color: Colors.greenAccent[400])),
                        const SizedBox(height: 4),
                        Text('Cloud Synced - Live', style: TextStyle(fontSize: 14, color: Colors.blueAccent, fontWeight: FontWeight.w500)),
                      ],
                    ),
                    Container(
                      padding: const EdgeInsets.all(10),
                      decoration: BoxDecoration(color: Colors.greenAccent.withOpacity(0.1), shape: BoxShape.circle),
                      child: Icon(Icons.eco, color: Colors.greenAccent[400], size: 28),
                    )
                  ],
                ),
                const SizedBox(height: 30),
                Expanded(
                  child: GridView.count(
                    crossAxisCount: 2,
                    crossAxisSpacing: 20,
                    mainAxisSpacing: 20,
                    childAspectRatio: 0.85,
                    children: [
                      _buildPremiumCard('PM 2.5', '$pm25Value', 'µg/m³', Icons.air, pm25Value >= 35.0, pm25CardColor),
                      _buildPremiumCard('Humidity', '${sensorData['humidity']}', '%', Icons.water_drop, isDry, isDry ? Colors.redAccent : Colors.cyanAccent),
                      _buildPremiumCard('Temperature', '${sensorData['temperature']}', '°C', Icons.thermostat, false, Colors.orangeAccent),
                      _buildPremiumCard('Moss Health', sensorData['moss_status'], '', Icons.health_and_safety, isDry, isDry ? Colors.redAccent : Colors.greenAccent),
                    ],
                  ),
                ),
                AnimatedContainer(
                  duration: const Duration(milliseconds: 500),
                  padding: const EdgeInsets.symmetric(horizontal: 20, vertical: 18),
                  decoration: BoxDecoration(
                    gradient: LinearGradient(
                      colors: isDry || pm25Value > 55.0
                          ? [Colors.redAccent.withOpacity(0.8), Colors.red.withOpacity(0.2)]
                          : [Colors.greenAccent.withOpacity(0.8), Colors.teal.withOpacity(0.2)],
                      begin: Alignment.topLeft,
                      end: Alignment.bottomRight,
                    ),
                    borderRadius: BorderRadius.circular(20),
                    boxShadow: [
                      BoxShadow(color: (isDry || pm25Value > 55.0 ? Colors.redAccent : Colors.greenAccent).withOpacity(0.3), blurRadius: 20, offset: const Offset(0, 10))
                    ],
                    border: Border.all(color: Colors.white.withOpacity(0.2), width: 1.5),
                  ),
                  child: Row(
                    children: [
                      Icon(isDry || pm25Value > 55.0 ? Icons.warning_rounded : Icons.check_circle_outline, color: Colors.white, size: 28),
                      const SizedBox(width: 16),
                      Expanded(
                        child: Text(
                          sensorData['humidity'] == 0 
                            ? 'Connecting to Edge Device...' 
                            : (isDry ? 'SYSTEM ALERT: Moss dehydration' : (pm25Value > 55.0 ? 'SYSTEM ALERT: Toxic PM2.5' : 'System running optimally')),
                          style: const TextStyle(color: Colors.white, fontSize: 16, fontWeight: FontWeight.w700, letterSpacing: 0.5),
                        ),
                      ),
                    ],
                  ),
                )
              ],
            ),
          ),
        ),
      ),
    );
  }

  Widget _buildPremiumCard(String title, String value, String unit, IconData icon, bool isWarning, Color activeColor) {
    return AnimatedContainer(
      duration: const Duration(milliseconds: 400),
      padding: const EdgeInsets.all(20),
      decoration: BoxDecoration(
        gradient: const LinearGradient(colors: [Color(0xFF222834), Color(0xFF171A21)], begin: Alignment.topLeft, end: Alignment.bottomRight),
        borderRadius: BorderRadius.circular(24),
        boxShadow: [BoxShadow(color: activeColor.withOpacity(isWarning ? 0.4 : 0.15), blurRadius: isWarning ? 25 : 15, spreadRadius: isWarning ? 3 : 0, offset: const Offset(0, 8))],
        border: Border.all(color: activeColor.withOpacity(isWarning ? 0.8 : 0.3), width: isWarning ? 2 : 1),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        mainAxisAlignment: MainAxisAlignment.spaceBetween,
        children: [
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Icon(icon, size: 32, color: activeColor),
              if (isWarning) Icon(Icons.error_outline, size: 24, color: activeColor),
            ],
          ),
          Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Text(title, style: TextStyle(color: Colors.grey[400], fontSize: 14, fontWeight: FontWeight.w600, letterSpacing: 0.5)),
              const SizedBox(height: 8),
              Row(
                crossAxisAlignment: CrossAxisAlignment.baseline,
                textBaseline: TextBaseline.alphabetic,
                children: [
                  Text(value, style: const TextStyle(color: Colors.white, fontSize: 32, fontWeight: FontWeight.w900)),
                  if (unit.isNotEmpty) Text(' $unit', style: TextStyle(color: Colors.grey[500], fontSize: 16, fontWeight: FontWeight.bold)),
                ],
              ),
            ],
          ),
        ],
      ),
    );
  }
}