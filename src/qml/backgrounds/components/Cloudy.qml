import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import QtQuick.Shapes 1.12
import org.kde.kirigami 2.11 as Kirigami

Item {
    id: baseRect
    anchors.fill: parent
    property color cloudColor: "white"
    property double unitSize: width / 100 // 100 is full width of screen
    property double heightUnitSize: height / 100
    
    Cloud {
        opacity: 0.5
        color: baseRect.cloudColor
        maxRadiusChange: unitSize * 3
        maxCoordChange: unitSize * 6
        minRadius: unitSize * 40
        minX: unitSize * 10
        minY: heightUnitSize * -3
    }
    Cloud {
        opacity: 0.5
        color: baseRect.cloudColor
        maxRadiusChange: unitSize * 2
        maxCoordChange: unitSize * 5
        minRadius: unitSize * 40
        minX: unitSize * 40
        minY: heightUnitSize * 20
    }
    Cloud {
        opacity: 0.5
        color: baseRect.cloudColor
        maxRadiusChange: unitSize * 3
        maxCoordChange: unitSize * 2
        minRadius: unitSize * 23
        minX: unitSize * 85
        minY: heightUnitSize * 7
    }
    Cloud {
        opacity: 0.5
        color: baseRect.cloudColor
        maxRadiusChange: unitSize * 2
        maxCoordChange: unitSize * 6
        minRadius: unitSize * 40
        minX: unitSize * 110
        minY: heightUnitSize * -6
    }
    Cloud {
        opacity: 0.5
        color: baseRect.cloudColor
        maxRadiusChange: unitSize * 3
        maxCoordChange: unitSize * 2
        minRadius: unitSize * 32
        minX: unitSize * 32
        minY: heightUnitSize * 6
    }
    Cloud {
        opacity: 0.6
        color: baseRect.cloudColor
        maxRadiusChange: unitSize * 3
        maxCoordChange: unitSize * 6
        minRadius: unitSize * 24
        minX: unitSize * 48
        minY: heightUnitSize * 10
    }
    Cloud {
        opacity: 0.5
        color: baseRect.cloudColor
        maxRadiusChange: unitSize * 5
        maxCoordChange: unitSize * 2
        minRadius: unitSize * 27
        minX: unitSize * 90
        minY: heightUnitSize * 17
    }
}
