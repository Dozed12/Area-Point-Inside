
// 1 - Calculate area
// 2 - Prune vertices that arent visible to point
// 3 - Find closest unpruned vertice
// 4 - Calculate bisection at that vertice
// 5 - Find which side of the bisection the point is on
// 6 - Break edge of the side and add the point as a vertex
// 7 - Calculate new area
// 8 - Compare areas and reach conclusion if point is inside or outside

/*TODO List
	- Think more about easier ways to find closest edge
	- Add more visual debug features
*/

var Xs = [];
var Ys = [];

var oldArea;

function setup() {

	createCanvas(windowWidth, windowHeight);
	textSize(20);

	//Polygon

	let oldVertexes = randomPolygon(400, 300,
									30, 150, 100, -100);
	Xs = oldVertexes[0];
	Ys = oldVertexes[1];

	//Old Area

	oldArea = area(Xs, Ys);

	mouseMoved();

}

//When mouse moves
function mouseMoved() {

	clear();

	//Draw Polygon
	push();
	fill(color(220,220,220));
	drawPoly(Xs, Ys);
	pop();

	//Point at mouse
	var pX = mouseX;
	var pY = mouseY;

	//Closest Edge
	var closest = closestEdge(pX, pY);

	//Break Edge and connect new vertex
	var newVertexes = breakEdge(closest, pX, pY, Xs, Ys);

	//Calculate new Area
	var newArea = area(newVertexes[0], newVertexes[1]);	

	//Compare areas
	push();
	var messageInfo = String("Original Area - New Area = Area Difference");
	var messageNumbers = String(oldArea + " - " + newArea + " = " + (oldArea - newArea));
	if(newArea > oldArea){
		messageResult = "Point is outside polygon";
		fill(200, 0, 0);
	}else{
		messageResult = "Point is inside polygon";
		fill(0, 200, 0);
	}

	text(messageInfo, 400, 580);
	text(messageNumbers, 400, 600);
	text(messageResult, 400, 620);
	pop();
	
}

//Break and join new Vertex
function breakEdge(closest, pX, pY, Xs, Ys){

	var nXs = [];
	var nYs = [];

	//Copy
	for (let i = 0; i < Xs.length; i++) {
		nXs.push(Xs[i]);
		nYs.push(Ys[i]);
	}	

	//Add
	nXs.splice(closest[1], 0, pX);
	nYs.splice(closest[1], 0, pY);

	return [nXs, nYs];

}

//Calculate Area
function area(Xs, Ys){
	let n = Xs.length;
	let area = 0.0;
	let j = n - 1;
	for (let i = 0; i < n; i++) {
		area += (Xs[j] + Xs[i]) * (Ys[j] - Ys[i]);
		j = i;
	}

	return Math.abs(area/2.0);

}

//Draw a polygon
function drawPoly(Xs, Ys){
	beginShape();
	for (let i = 0; i < Xs.length; i++) {
		vertex(Xs[i], Ys[i]);
	}
	endShape(CLOSE);
}

//Closest edge to point
function closestEdge(x, y){
	
	//Copy new structure
	var vertices = [];
	for (let i = 0; i < Xs.length; i++) {
		vertices.push({id:i, x:Xs[i], y:Ys[i]});
	}		

	//Identify vertices to prune
	//Prune vertices that line to target point pass through the polygon
	var toPrune = [];
	for (let i = 0; i < vertices.length; i++) {		
		let intersections = 0;
		for (let j = 0; j < Xs.length-1; j++) {
			intersections += intersection(vertices[i].x, vertices[i].y, x, y, Xs[j], Ys[j], Xs[j+1], Ys[j+1]);
			//Too many intersections already, prune
			if(intersections > 2)
				break;
		}
		//Check first and last vertex
		if(intersections <= 2)
			intersections += intersection(vertices[i].x, vertices[i].y, x, y, Xs[Xs.length-1], Ys[Xs.length-1], Xs[0], Ys[0]);
		//Too many intersections, prune
		if(intersections > 2)
			toPrune.push(vertices[i].id);
	}

	//Prune
	for (let p = toPrune.length - 1; p >= 0; p--) {
		vertices.splice(toPrune[p],1);
	}

	//DEBUG Remaining vertices
	push();
	fill(color(125,125,125));
	for (let i = 0; i < vertices.length; i++) {
		ellipse(vertices[i].x, vertices[i].y, 10, 10);		
	}
	pop();

	//Choose closest of remaining vertices
	let minDistToVertex = Number.MAX_SAFE_INTEGER;
	var closestVertexId = 0;
	for (let i = 0; i < vertices.length; i++) {
		let distToVertex = dist(x, y, vertices[i].x, vertices[i].y);
		if(minDistToVertex > distToVertex){
			closestVertexId = vertices[i].id;
			minDistToVertex = distToVertex;
		}
	}

	//IDs
	var prevID;
	var nextID;

	nextID = closestVertexId + 1;
	if(closestVertexId == Xs.length - 1)
		nextID = 0;

	prevID = closestVertexId - 1;
	if(closestVertexId == 0)
		prevID = Xs.length - 1;

	//Coordinates of neighbor vertices
	var vertexX = Xs[closestVertexId];
	var vertexY = Ys[closestVertexId];
	var prevX = Xs[prevID];
	var prevY = Ys[prevID];
	var nextX = Xs[nextID];
	var nextY = Ys[nextID];

	//Vectors to neighbor vertices
	var prevVec = createVector(vertexX - prevX, vertexY - prevY);
	var nextVec = createVector(nextX - vertexX, nextY - vertexY);

	//DEBUG Vectors
	push();
	translate(vertexX,vertexY);
	stroke(color(0,255,0));
	strokeWeight(5);
	line(0,0,nextVec.x ,nextVec.y );
	line(0,0,-prevVec.x ,-prevVec.y );
	pop();	

	//Bisector vector between neighbor vertices
	var magPrev = prevVec.mag();
	var magNext = nextVec.mag();
	prevVec.mult(magNext);
	nextVec.mult(magPrev);
	var bisectorVector = p5.Vector.add(prevVec, nextVec)
	bisectorVector.rotate(Math.PI/2);

	//DEBUG BISECTOR
	push();
	translate(vertexX,vertexY);
	stroke(color(255,0,0));
	line(bisectorVector.x*100,bisectorVector.y*100,-bisectorVector.x*100,-bisectorVector.y*100);
	pop();	

	//Check which side the point is of the bisector
	var pointLineX = vertexX + bisectorVector.x;
	var pointLineY = vertexY + bisectorVector.y;
	var position = Math.sign((pointLineX - vertexX) * (y - vertexY) - (pointLineY - vertexY) * (x - vertexX));

	if(position == 1){
		//DEBUG New Triangle
		push();
		fill(color(50,50,50));
		triangle(prevX, prevY, x, y, vertexX, vertexY);
		pop();
		return [prevID, closestVertexId];
	}else{
		//DEBUG New Triangle
		push();
		fill(color(50,50,50));
		triangle(vertexX, vertexY, x, y, nextX, nextY);
		pop();
		return [closestVertexId, nextID];
	}

}

//Generate a random concave polygon
function randomPolygon(centerX, centerY, div, radius, maxUpDelta, maxDownDelta){
	var vertexes = [];

	var Xs = [];
	var Ys = [];

	let deltaAngle = 2 * Math.PI / div;
	for (let angle = 0; angle < Math.PI * 2; angle += deltaAngle) {
		let delta = random(maxDownDelta, maxUpDelta+1);
		Xs.push(centerX + Math.cos(angle) * (radius + delta));
		Ys.push(centerY + Math.sin(angle) * (radius + delta));
	}

	vertexes.push(Xs);
	vertexes.push(Ys);

	return vertexes;
}

//Calculate intersection of two lines.
function intersection(start1x, start1y, end1x, end1y, start2x, start2y, end2x, end2y){

	let ax = end1x - start1x;
	let ay = end1y - start1y;

	let bx = start2x - end2x;
	let by = start2y - end2y;

	let dx = start2x - start1x;
	let dy = start2y - start1y;

	let det = ax * by - ay * bx;

	if (det == 0) return false;

	let r = (dx * by - dy * bx) / det;
	let s = (ax * dy - ay * dx) / det;

	return !(r < 0 || r > 1 || s < 0 || s > 1);

}